/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "php_driver.h"
#include "php_driver_globals.h"
#include "php_driver_types.h"
#include "version.h"

#include "util/types.h"
#include "util/ref.h"

#include <php_ini.h>
#include <ext/standard/info.h>

#ifndef _WIN32
#include <php_syslog.h>
#else
#pragma message("syslog will be disabled on Windows")
#endif
#include <ext/standard/info.h>
#include <fcntl.h>
#include <uv.h>

/* Resources */
#define PHP_CASSANDRA_CLUSTER_RES_NAME "Cassandra Cluster"
#define PHP_CASSANDRA_SESSION_RES_NAME "Cassandra Session"

static uv_once_t log_once = UV_ONCE_INIT;
static char *log_location = NULL;
static uv_rwlock_t log_lock;

#if CURRENT_CPP_DRIVER_VERSION < CPP_DRIVER_VERSION(2, 5, 0)
#error C/C++ driver version 2.5.0 or greater required
#endif

ZEND_DECLARE_MODULE_GLOBALS(php_driver)

static PHP_GINIT_FUNCTION(php_driver);
static PHP_GSHUTDOWN_FUNCTION(php_driver);

const zend_function_entry php_driver_functions[] = {
  PHP_FE_END /* Must be the last line in php_driver_functions[] */
};

#if ZEND_MODULE_API_NO >= 20050617
static zend_module_dep php_driver_deps[] = {
  ZEND_MOD_REQUIRED("spl")
  ZEND_MOD_END
};
#endif

zend_module_entry php_driver_module_entry = {
#if ZEND_MODULE_API_NO >= 20050617
  STANDARD_MODULE_HEADER_EX, NULL, php_driver_deps,
#elif ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_CASSANDRA_NAME,
  php_driver_functions,      /* Functions */
  PHP_MINIT(php_driver),     /* MINIT */
  PHP_MSHUTDOWN(php_driver), /* MSHUTDOWN */
  PHP_RINIT(php_driver),     /* RINIT */
  PHP_RSHUTDOWN(php_driver), /* RSHUTDOWN */
  PHP_MINFO(php_driver),     /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
  PHP_CASSANDRA_VERSION,
#endif
  PHP_MODULE_GLOBALS(php_driver),
  PHP_GINIT(php_driver),
  PHP_GSHUTDOWN(php_driver),
  NULL,
  STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_CASSANDRA
ZEND_GET_MODULE(php_driver)
#endif

PHP_INI_BEGIN()
  PHP_CASSANDRA_INI_ENTRY_LOG
  PHP_CASSANDRA_INI_ENTRY_LOG_LEVEL
PHP_INI_END()

static int le_cassandra_cluster_res;
int
php_le_cassandra_cluster()
{
  return le_cassandra_cluster_res;
}
static void
php_cassandra_cluster_dtor(php5to7_zend_resource rsrc TSRMLS_DC)
{
  CassCluster *cluster = (CassCluster*) rsrc->ptr;

  if (cluster) {
    cass_cluster_free(cluster);
    PHP_DRIVER_G(persistent_clusters)--;
    rsrc->ptr = NULL;
  }
}

static int le_cassandra_session_res;
int
php_le_cassandra_session()
{
  return le_cassandra_session_res;
}
static void
php_cassandra_session_dtor(php5to7_zend_resource rsrc TSRMLS_DC)
{
  cassandra_psession *psession = (cassandra_psession*) rsrc->ptr;

  if (psession) {
    cass_future_free(psession->future);
    php_cassandra_del_peref(&psession->session, 1);
    pefree(psession, 1);
    PHP_DRIVER_G(persistent_sessions)--;
    rsrc->ptr = NULL;
  }
}

static void
php_cassandra_log(const CassLogMessage *message, void *data);

static void
php_cassandra_log_cleanup()
{
  cass_log_cleanup();
  uv_rwlock_destroy(&log_lock);
  if (log_location) {
    free(log_location);
    log_location = NULL;
  }
}

static void
php_cassandra_log_initialize()
{
  uv_rwlock_init(&log_lock);
  cass_log_set_level(CASS_LOG_ERROR);
  cass_log_set_callback(php_cassandra_log, NULL);
}

static void
php_cassandra_log(const CassLogMessage *message, void *data)
{
  char log[MAXPATHLEN + 1];
  uint log_length = 0;

  /* Making a copy here because location could be updated by a PHP thread. */
  uv_rwlock_rdlock(&log_lock);
  if (log_location) {
    log_length = MIN(strlen(log_location), MAXPATHLEN);
    memcpy(log, log_location, log_length);
  }
  uv_rwlock_rdunlock(&log_lock);

  log[log_length] = '\0';

  if (log_length > 0) {
    FILE *fd = NULL;
#ifndef _WIN32
    if (!strcmp(log, "syslog")) {
      php_syslog(LOG_NOTICE, "cassandra | [%s] %s (%s:%d)",
                 cass_log_level_string(message->severity), message->message,
                 message->file, message->line);
      return;
    }
#endif

    fd = fopen(log, "a");
    if (fd) {
      time_t log_time;
      struct tm log_tm;
      char log_time_str[64];
      size_t needed = 0;
      char *tmp     = NULL;

      time(&log_time);
      php_localtime_r(&log_time, &log_tm);
      strftime(log_time_str, sizeof(log_time_str), "%d-%m-%Y %H:%M:%S %Z", &log_tm);

      needed = snprintf(NULL, 0, "%s [%s] %s (%s:%d)%s",
                        log_time_str,
                        cass_log_level_string(message->severity), message->message,
                        message->file, message->line,
                        PHP_EOL);

      tmp = malloc(needed + 1);
      sprintf(tmp, "%s [%s] %s (%s:%d)%s",
              log_time_str,
              cass_log_level_string(message->severity), message->message,
              message->file, message->line,
              PHP_EOL);

      fwrite(tmp, 1, needed, fd);
      free(tmp);
      fclose(fd);
      return;
    }
  }

  /* This defaults to using "stderr" instead of "sapi_module.log_message"
   * because there are no guarantees that all implementations of the SAPI
   * logging function are thread-safe.
   */

  fprintf(stderr, "cassandra | [%s] %s (%s:%d)%s",
          cass_log_level_string(message->severity), message->message,
          message->file, message->line,
          PHP_EOL);
}

zend_class_entry*
exception_class(CassError rc)
{
  switch (rc) {
  case CASS_ERROR_LIB_BAD_PARAMS:
  case CASS_ERROR_LIB_INDEX_OUT_OF_BOUNDS:
  case CASS_ERROR_LIB_INVALID_ITEM_COUNT:
  case CASS_ERROR_LIB_INVALID_VALUE_TYPE:
  case CASS_ERROR_LIB_INVALID_STATEMENT_TYPE:
  case CASS_ERROR_LIB_NAME_DOES_NOT_EXIST:
  case CASS_ERROR_LIB_NULL_VALUE:
  case CASS_ERROR_SSL_INVALID_CERT:
  case CASS_ERROR_SSL_INVALID_PRIVATE_KEY:
  case CASS_ERROR_SSL_NO_PEER_CERT:
  case CASS_ERROR_SSL_INVALID_PEER_CERT:
  case CASS_ERROR_SSL_IDENTITY_MISMATCH:
    return cassandra_invalid_argument_exception_ce;
  case CASS_ERROR_LIB_NO_STREAMS:
  case CASS_ERROR_LIB_UNABLE_TO_INIT:
  case CASS_ERROR_LIB_MESSAGE_ENCODE:
  case CASS_ERROR_LIB_HOST_RESOLUTION:
  case CASS_ERROR_LIB_UNEXPECTED_RESPONSE:
  case CASS_ERROR_LIB_REQUEST_QUEUE_FULL:
  case CASS_ERROR_LIB_NO_AVAILABLE_IO_THREAD:
  case CASS_ERROR_LIB_WRITE_ERROR:
  case CASS_ERROR_LIB_NO_HOSTS_AVAILABLE:
  case CASS_ERROR_LIB_UNABLE_TO_SET_KEYSPACE:
  case CASS_ERROR_LIB_UNABLE_TO_DETERMINE_PROTOCOL:
  case CASS_ERROR_LIB_UNABLE_TO_CONNECT:
  case CASS_ERROR_LIB_UNABLE_TO_CLOSE:
    return cassandra_runtime_exception_ce;
  case CASS_ERROR_LIB_REQUEST_TIMED_OUT:
    return cassandra_timeout_exception_ce;
  case CASS_ERROR_LIB_CALLBACK_ALREADY_SET:
  case CASS_ERROR_LIB_NOT_IMPLEMENTED:
    return cassandra_logic_exception_ce;
  case CASS_ERROR_SERVER_SERVER_ERROR:
    return cassandra_server_exception_ce;
  case CASS_ERROR_SERVER_PROTOCOL_ERROR:
    return cassandra_protocol_exception_ce;
  case CASS_ERROR_SERVER_BAD_CREDENTIALS:
    return cassandra_authentication_exception_ce;
  case CASS_ERROR_SERVER_UNAVAILABLE:
    return cassandra_unavailable_exception_ce;
  case CASS_ERROR_SERVER_OVERLOADED:
    return cassandra_overloaded_exception_ce;
  case CASS_ERROR_SERVER_IS_BOOTSTRAPPING:
    return cassandra_is_bootstrapping_exception_ce;
  case CASS_ERROR_SERVER_TRUNCATE_ERROR:
    return cassandra_truncate_exception_ce;
  case CASS_ERROR_SERVER_WRITE_TIMEOUT:
    return cassandra_write_timeout_exception_ce;
  case CASS_ERROR_SERVER_READ_TIMEOUT:
    return cassandra_read_timeout_exception_ce;
  case CASS_ERROR_SERVER_SYNTAX_ERROR:
    return cassandra_invalid_syntax_exception_ce;
  case CASS_ERROR_SERVER_UNAUTHORIZED:
    return cassandra_unauthorized_exception_ce;
  case CASS_ERROR_SERVER_INVALID_QUERY:
    return cassandra_invalid_query_exception_ce;
  case CASS_ERROR_SERVER_CONFIG_ERROR:
    return cassandra_configuration_exception_ce;
  case CASS_ERROR_SERVER_ALREADY_EXISTS:
    return cassandra_already_exists_exception_ce;
  case CASS_ERROR_SERVER_UNPREPARED:
    return cassandra_unprepared_exception_ce;
  default:
    return cassandra_runtime_exception_ce;
  }
}

void
throw_invalid_argument(zval *object,
                       const char *object_name,
                       const char *expected_type TSRMLS_DC)
{
  if (Z_TYPE_P(object) == IS_OBJECT) {
#if ZEND_MODULE_API_NO >= 20100525
    const char* cls_name = NULL;
#else
    char* cls_name = NULL;
#endif

#if PHP_MAJOR_VERSION >= 7
    size_t cls_len;
#else
    zend_uint cls_len;
#endif

#if PHP_MAJOR_VERSION >= 7
    zend_string* str  = Z_OBJ_HANDLER_P(object, get_class_name)(Z_OBJ_P(object) TSRMLS_CC);
    cls_name = str->val;
    cls_len = str->len;
#else
    Z_OBJ_HANDLER_P(object, get_class_name)(object, &cls_name, &cls_len, 0 TSRMLS_CC);
#endif
    if (cls_name) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "%s must be %s, an instance of %.*s given",
                              object_name, expected_type, cls_len, cls_name);
#if PHP_MAJOR_VERSION >= 7
      zend_string_release(str);
#else
      efree((void*) cls_name);
#endif
    } else {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "%s must be %s, an instance of Unknown Class given",
                              object_name, expected_type);
    }
  } else if (Z_TYPE_P(object) == IS_STRING) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "%s must be %s, '%Z' given",
                            object_name, expected_type, object);
  } else {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "%s must be %s, %Z given",
                            object_name, expected_type, object);
  }
}

PHP_INI_MH(OnUpdateLogLevel)
{
  /* If TSRM is enabled then the last thread to update this wins */

  if (new_value) {
    if (PHP5TO7_STRCMP(new_value, "CRITICAL") == 0) {
      cass_log_set_level(CASS_LOG_DISABLED);
    } else if (PHP5TO7_STRCMP(new_value, "ERROR") == 0) {
      cass_log_set_level(CASS_LOG_ERROR);
    } else if (PHP5TO7_STRCMP(new_value, "WARN") == 0) {
      cass_log_set_level(CASS_LOG_WARN);
    } else if (PHP5TO7_STRCMP(new_value, "INFO") == 0) {
      cass_log_set_level(CASS_LOG_INFO);
    } else if (PHP5TO7_STRCMP(new_value, "DEBUG") == 0) {
      cass_log_set_level(CASS_LOG_DEBUG);
    } else if (PHP5TO7_STRCMP(new_value, "TRACE") == 0) {
      cass_log_set_level(CASS_LOG_TRACE);
    } else {
      php_error_docref(NULL TSRMLS_CC, E_NOTICE,
                       "cassandra | Unknown log level '%s', using 'ERROR'",
                       new_value);
      cass_log_set_level(CASS_LOG_ERROR);
    }
  }

  return SUCCESS;
}

PHP_INI_MH(OnUpdateLog)
{
  /* If TSRM is enabled then the last thread to update this wins */

  uv_rwlock_wrlock(&log_lock);
  if (log_location) {
    free(log_location);
    log_location = NULL;
  }
  if (new_value) {
    if (PHP5TO7_STRCMP(new_value, "syslog") != 0) {
      char realpath[MAXPATHLEN + 1];
      if (VCWD_REALPATH(PHP5TO7_STRVAL(new_value), realpath)) {
        log_location = strdup(realpath);
      } else {
        log_location = strdup(PHP5TO7_STRVAL(new_value));
      }
    } else {
      log_location = strdup(PHP5TO7_STRVAL(new_value));
    }
  }
  uv_rwlock_wrunlock(&log_lock);

  return SUCCESS;
}


static PHP_GINIT_FUNCTION(php_driver)
{
  uv_once(&log_once, php_cassandra_log_initialize);

  PHP_DRIVER_G(uuid_gen)            = NULL;
  PHP_DRIVER_G(uuid_gen_pid)        = 0;
  PHP_DRIVER_G(persistent_clusters) = 0;
  PHP_DRIVER_G(persistent_sessions) = 0;
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_varchar));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_text));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_blob));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_ascii));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_bigint));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_smallint));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_counter));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_int));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_varint));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_boolean));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_decimal));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_double));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_float));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_inet));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_timestamp));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_uuid));
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_timeuuid));
}

static PHP_GSHUTDOWN_FUNCTION(php_driver)
{
  if (PHP_DRIVER_G(uuid_gen)) {
    cass_uuid_gen_free(PHP_DRIVER_G(uuid_gen));
  }
  php_cassandra_log_cleanup();
}

PHP_MINIT_FUNCTION(php_driver)
{
  REGISTER_INI_ENTRIES();

  le_cassandra_cluster_res =
  zend_register_list_destructors_ex(NULL, php_cassandra_cluster_dtor,
                                    PHP_CASSANDRA_CLUSTER_RES_NAME,
                                    module_number);
  le_cassandra_session_res =
  zend_register_list_destructors_ex(NULL, php_cassandra_session_dtor,
                                    PHP_CASSANDRA_SESSION_RES_NAME,
                                    module_number);

  cassandra_define_Exception(TSRMLS_C);
  cassandra_define_InvalidArgumentException(TSRMLS_C);
  cassandra_define_DomainException(TSRMLS_C);
  cassandra_define_RuntimeException(TSRMLS_C);
  cassandra_define_TimeoutException(TSRMLS_C);
  cassandra_define_LogicException(TSRMLS_C);
  cassandra_define_ExecutionException(TSRMLS_C);
  cassandra_define_ReadTimeoutException(TSRMLS_C);
  cassandra_define_WriteTimeoutException(TSRMLS_C);
  cassandra_define_UnavailableException(TSRMLS_C);
  cassandra_define_TruncateException(TSRMLS_C);
  cassandra_define_ValidationException(TSRMLS_C);
  cassandra_define_InvalidQueryException(TSRMLS_C);
  cassandra_define_InvalidSyntaxException(TSRMLS_C);
  cassandra_define_UnauthorizedException(TSRMLS_C);
  cassandra_define_UnpreparedException(TSRMLS_C);
  cassandra_define_ConfigurationException(TSRMLS_C);
  cassandra_define_AlreadyExistsException(TSRMLS_C);
  cassandra_define_AuthenticationException(TSRMLS_C);
  cassandra_define_ProtocolException(TSRMLS_C);
  cassandra_define_ServerException(TSRMLS_C);
  cassandra_define_IsBootstrappingException(TSRMLS_C);
  cassandra_define_OverloadedException(TSRMLS_C);
  cassandra_define_RangeException(TSRMLS_C);
  cassandra_define_DivideByZeroException(TSRMLS_C);

  cassandra_define_Value(TSRMLS_C);
  cassandra_define_Numeric(TSRMLS_C);
  cassandra_define_Bigint(TSRMLS_C);
  cassandra_define_Smallint(TSRMLS_C);
  cassandra_define_Tinyint(TSRMLS_C);
  cassandra_define_Blob(TSRMLS_C);
  cassandra_define_Decimal(TSRMLS_C);
  cassandra_define_Float(TSRMLS_C);
  cassandra_define_Inet(TSRMLS_C);
  cassandra_define_Timestamp(TSRMLS_C);
  cassandra_define_Date(TSRMLS_C);
  cassandra_define_Time(TSRMLS_C);
  cassandra_define_UuidInterface(TSRMLS_C);
  cassandra_define_Timeuuid(TSRMLS_C);
  cassandra_define_Uuid(TSRMLS_C);
  cassandra_define_Varint(TSRMLS_C);
  cassandra_define_Custom(TSRMLS_C);

  cassandra_define_Set(TSRMLS_C);
  cassandra_define_Map(TSRMLS_C);
  cassandra_define_Collection(TSRMLS_C);
  cassandra_define_Tuple(TSRMLS_C);
  cassandra_define_UserTypeValue(TSRMLS_C);

  cassandra_define_Cassandra(TSRMLS_C);
  cassandra_define_Cluster(TSRMLS_C);
  cassandra_define_DefaultCluster(TSRMLS_C);
  cassandra_define_ClusterBuilder(TSRMLS_C);
  cassandra_define_Future(TSRMLS_C);
  cassandra_define_FuturePreparedStatement(TSRMLS_C);
  cassandra_define_FutureRows(TSRMLS_C);
  cassandra_define_FutureSession(TSRMLS_C);
  cassandra_define_FutureValue(TSRMLS_C);
  cassandra_define_FutureClose(TSRMLS_C);
  cassandra_define_Session(TSRMLS_C);
  cassandra_define_DefaultSession(TSRMLS_C);
  cassandra_define_SSLOptions(TSRMLS_C);
  cassandra_define_SSLOptionsBuilder(TSRMLS_C);
  cassandra_define_Statement(TSRMLS_C);
  cassandra_define_SimpleStatement(TSRMLS_C);
  cassandra_define_PreparedStatement(TSRMLS_C);
  cassandra_define_BatchStatement(TSRMLS_C);
  cassandra_define_ExecutionOptions(TSRMLS_C);
  cassandra_define_Rows(TSRMLS_C);

  cassandra_define_Schema(TSRMLS_C);
  cassandra_define_DefaultSchema(TSRMLS_C);
  cassandra_define_Keyspace(TSRMLS_C);
  cassandra_define_DefaultKeyspace(TSRMLS_C);
  cassandra_define_Table(TSRMLS_C);
  cassandra_define_DefaultTable(TSRMLS_C);
  cassandra_define_Column(TSRMLS_C);
  cassandra_define_DefaultColumn(TSRMLS_C);
  cassandra_define_Index(TSRMLS_C);
  cassandra_define_DefaultIndex(TSRMLS_C);
  cassandra_define_MaterializedView(TSRMLS_C);
  cassandra_define_DefaultMaterializedView(TSRMLS_C);
  cassandra_define_Function(TSRMLS_C);
  cassandra_define_DefaultFunction(TSRMLS_C);
  cassandra_define_Aggregate(TSRMLS_C);
  cassandra_define_DefaultAggregate(TSRMLS_C);

  cassandra_define_Type(TSRMLS_C);
  cassandra_define_TypeScalar(TSRMLS_C);
  cassandra_define_TypeCollection(TSRMLS_C);
  cassandra_define_TypeSet(TSRMLS_C);
  cassandra_define_TypeMap(TSRMLS_C);
  cassandra_define_TypeTuple(TSRMLS_C);
  cassandra_define_TypeUserType(TSRMLS_C);
  cassandra_define_TypeCustom(TSRMLS_C);

  cassandra_define_RetryPolicy(TSRMLS_C);
  cassandra_define_RetryPolicyDefault(TSRMLS_C);
  cassandra_define_RetryPolicyDowngradingConsistency(TSRMLS_C);
  cassandra_define_RetryPolicyFallthrough(TSRMLS_C);
  cassandra_define_RetryPolicyLogging(TSRMLS_C);

  cassandra_define_TimestampGenerator(TSRMLS_C);
  cassandra_define_TimestampGeneratorMonotonic(TSRMLS_C);
  cassandra_define_TimestampGeneratorServerSide(TSRMLS_C);

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(php_driver)
{
  /* UNREGISTER_INI_ENTRIES(); */

  return SUCCESS;
}

PHP_RINIT_FUNCTION(php_driver)
{
#define XX_SCALAR(name, value) \
  PHP5TO7_ZVAL_UNDEF(PHP_DRIVER_G(type_##name));

  PHP_CASSANDRA_SCALAR_TYPES_MAP(XX_SCALAR)
#undef XX_SCALAR

  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(php_driver)
{
#define XX_SCALAR(name, value) \
  PHP5TO7_ZVAL_MAYBE_DESTROY(PHP_DRIVER_G(type_##name));

  PHP_CASSANDRA_SCALAR_TYPES_MAP(XX_SCALAR)
#undef XX_SCALAR

  return SUCCESS;
}

PHP_MINFO_FUNCTION(php_driver)
{
  char buf[256];
  php_info_print_table_start();
  php_info_print_table_header(2, "Cassandra support", "enabled");

  snprintf(buf, sizeof(buf), "%d.%d.%d%s",
           CASS_VERSION_MAJOR, CASS_VERSION_MINOR, CASS_VERSION_PATCH,
           strlen(CASS_VERSION_SUFFIX) > 0 ? "-" CASS_VERSION_SUFFIX : "");
  php_info_print_table_row(2, "C/C++ driver version", buf);

  snprintf(buf, sizeof(buf), "%d", PHP_DRIVER_G(persistent_clusters));
  php_info_print_table_row(2, "Persistent Clusters", buf);

  snprintf(buf, sizeof(buf), "%d", PHP_DRIVER_G(persistent_sessions));
  php_info_print_table_row(2, "Persistent Sessions", buf);

  php_info_print_table_end();

  DISPLAY_INI_ENTRIES();
}
