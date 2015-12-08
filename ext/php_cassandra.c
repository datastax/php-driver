#include "php_cassandra.h"
#include <php_ini.h>
#ifndef _WIN32
#include <php_syslog.h>
#else
#pragma message("syslog will be disabled on Windows")
#endif
#include <ext/standard/info.h>
#include <fcntl.h>
#include <uv.h>

#include "util/types.h"

#define PHP_CASSANDRA_DEFAULT_LOG       "cassandra.log"
#define PHP_CASSANDRA_DEFAULT_LOG_LEVEL "ERROR"

static uv_once_t log_once = UV_ONCE_INIT;
static char* log_location = NULL;
static uv_rwlock_t log_lock;

ZEND_DECLARE_MODULE_GLOBALS(cassandra)
static PHP_GINIT_FUNCTION(cassandra);
static PHP_GSHUTDOWN_FUNCTION(cassandra);

const zend_function_entry cassandra_functions[] = {
  PHP_FE_END /* Must be the last line in cassandra_functions[] */
};

#if ZEND_MODULE_API_NO >= 20050617
static zend_module_dep php_cassandra_deps[] = {
  ZEND_MOD_REQUIRED("spl")
  ZEND_MOD_END
};
#endif

zend_module_entry cassandra_module_entry = {
#if ZEND_MODULE_API_NO >= 20050617
  STANDARD_MODULE_HEADER_EX, NULL, php_cassandra_deps,
#elif ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_CASSANDRA_NAME,
  cassandra_functions,      /* Functions */
  PHP_MINIT(cassandra),     /* MINIT */
  PHP_MSHUTDOWN(cassandra), /* MSHUTDOWN */
  PHP_RINIT(cassandra),     /* RINIT */
  PHP_RSHUTDOWN(cassandra), /* RSHUTDOWN */
  PHP_MINFO(cassandra),     /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
  PHP_CASSANDRA_VERSION,
#endif
  PHP_MODULE_GLOBALS(cassandra),
  PHP_GINIT(cassandra),
  PHP_GSHUTDOWN(cassandra),
  NULL,
  STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_CASSANDRA
ZEND_GET_MODULE(cassandra)
#endif

static void
php_cassandra_log(const CassLogMessage* message, void* data);

void
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
php_cassandra_log(const CassLogMessage* message, void* data)
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
    int fd = -1;
#ifndef _WIN32
    if (!strcmp(log, "syslog")) {
      php_syslog(LOG_NOTICE, "cassandra | [%s] %s (%s:%d)",
                 cass_log_level_string(message->severity), message->message,
                 message->file, message->line);
      return;
    }
#endif

    fd = open(log, O_CREAT | O_APPEND | O_WRONLY, 0644);

    if (fd != 1) {
      time_t log_time;
      struct tm log_tm;
      char log_time_str[32];
      size_t needed = 0;
      char* tmp     = NULL;

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

      write(fd, tmp, needed);
      free(tmp);
      close(fd);
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

static int le_cassandra_cluster_res;
int
php_le_cassandra_cluster()
{
  return le_cassandra_cluster_res;
}
static void
php_cassandra_cluster_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassCluster* cluster = (CassCluster*) rsrc->ptr;

  if (cluster) {
    cass_cluster_free(cluster);
    CASSANDRA_G(persistent_clusters)--;
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
php_cassandra_session_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  cassandra_psession* psession = (cassandra_psession*) rsrc->ptr;

  if (psession) {
    cass_future_free(psession->future);
    cass_session_free(psession->session);
    pefree(psession, 1);
    CASSANDRA_G(persistent_sessions)--;
    rsrc->ptr = NULL;
  }
}

static PHP_INI_MH(OnUpdateLogLevel)
{
  /* If TSRM is enabled then the last thread to update this wins */

  if (new_value) {
    if (strcmp(new_value, "CRITICAL") == 0) {
      cass_log_set_level(CASS_LOG_DISABLED);
    } else if (strcmp(new_value, "ERROR") == 0) {
      cass_log_set_level(CASS_LOG_ERROR);
    } else if (strcmp(new_value, "WARN") == 0) {
      cass_log_set_level(CASS_LOG_WARN);
    } else if (strcmp(new_value, "INFO") == 0) {
      cass_log_set_level(CASS_LOG_INFO);
    } else if (strcmp(new_value, "DEBUG") == 0) {
      cass_log_set_level(CASS_LOG_DEBUG);
    } else if (strcmp(new_value, "TRACE") == 0) {
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

static PHP_INI_MH(OnUpdateLog)
{
  /* If TSRM is enabled then the last thread to update this wins */

  uv_rwlock_wrlock(&log_lock);
  if (log_location) {
    free(log_location);
    log_location = NULL;
  }
  if (new_value) {
    if (strcmp(new_value, "syslog") != 0) {
      char realpath[MAXPATHLEN + 1];
      if (VCWD_REALPATH(new_value, realpath)) {
        log_location = strdup(realpath);
      } else {
        log_location = strdup(new_value);
      }
    } else {
      log_location = strdup(new_value);
    }
  }
  uv_rwlock_wrunlock(&log_lock);

  return SUCCESS;
}

PHP_INI_BEGIN()
PHP_INI_ENTRY("cassandra.log",       PHP_CASSANDRA_DEFAULT_LOG,       PHP_INI_ALL, OnUpdateLog)
PHP_INI_ENTRY("cassandra.log_level", PHP_CASSANDRA_DEFAULT_LOG_LEVEL, PHP_INI_ALL, OnUpdateLogLevel)
PHP_INI_END()

static PHP_GINIT_FUNCTION(cassandra)
{
  uv_once(&log_once, php_cassandra_log_initialize);

  cassandra_globals->uuid_gen            = cass_uuid_gen_new();
  cassandra_globals->persistent_clusters = 0;
  cassandra_globals->persistent_sessions = 0;
  cassandra_globals->type_varchar        = NULL;
  cassandra_globals->type_text           = NULL;
  cassandra_globals->type_blob           = NULL;
  cassandra_globals->type_ascii          = NULL;
  cassandra_globals->type_bigint         = NULL;
  cassandra_globals->type_counter        = NULL;
  cassandra_globals->type_int            = NULL;
  cassandra_globals->type_varint         = NULL;
  cassandra_globals->type_boolean        = NULL;
  cassandra_globals->type_decimal        = NULL;
  cassandra_globals->type_double         = NULL;
  cassandra_globals->type_float          = NULL;
  cassandra_globals->type_inet           = NULL;
  cassandra_globals->type_timestamp      = NULL;
  cassandra_globals->type_uuid           = NULL;
  cassandra_globals->type_timeuuid       = NULL;
}

static PHP_GSHUTDOWN_FUNCTION(cassandra)
{
  cass_uuid_gen_free(cassandra_globals->uuid_gen);
  php_cassandra_log_cleanup();
}

PHP_MINIT_FUNCTION(cassandra)
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
  cassandra_define_Blob(TSRMLS_C);
  cassandra_define_Decimal(TSRMLS_C);
  cassandra_define_Float(TSRMLS_C);
  cassandra_define_Inet(TSRMLS_C);
  cassandra_define_Timestamp(TSRMLS_C);
  cassandra_define_UuidInterface(TSRMLS_C);
  cassandra_define_Timeuuid(TSRMLS_C);
  cassandra_define_Uuid(TSRMLS_C);
  cassandra_define_Varint(TSRMLS_C);

  cassandra_define_Set(TSRMLS_C);
  cassandra_define_Map(TSRMLS_C);
  cassandra_define_Collection(TSRMLS_C);

  cassandra_define_Cassandra(TSRMLS_C);
  cassandra_define_Cluster(TSRMLS_C);
  cassandra_define_ClusterBuilder(TSRMLS_C);
  cassandra_define_DefaultCluster(TSRMLS_C);
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

  cassandra_define_Type(TSRMLS_C);
  cassandra_define_TypeScalar(TSRMLS_C);
  cassandra_define_TypeCollection(TSRMLS_C);
  cassandra_define_TypeSet(TSRMLS_C);
  cassandra_define_TypeMap(TSRMLS_C);
  cassandra_define_TypeCustom(TSRMLS_C);

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cassandra)
{
  /* UNREGISTER_INI_ENTRIES(); */

  return SUCCESS;
}

PHP_RINIT_FUNCTION(cassandra)
{
  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(cassandra)
{
#define TYPE_CODE(m) type_ ## m
#define XX_SCALAR(name, value) \
    if (CASSANDRA_G(TYPE_CODE(name)) != NULL) { \
      zval_ptr_dtor(&CASSANDRA_G(TYPE_CODE(name))); \
      CASSANDRA_G(TYPE_CODE(name)) = NULL; \
    }

  PHP_CASSANDRA_SCALAR_TYPES_MAP(XX_SCALAR)
#undef XX_SCALAR
#undef TYPE_CODE

  return SUCCESS;
}

PHP_MINFO_FUNCTION(cassandra)
{
  char buf[256];
  php_info_print_table_start();
  php_info_print_table_header(2, "Cassandra support", "enabled");

  snprintf(buf, sizeof(buf), "%d", CASSANDRA_G(persistent_clusters));
  php_info_print_table_row(2, "Persistent Clusters", buf);

  snprintf(buf, sizeof(buf), "%d", CASSANDRA_G(persistent_sessions));
  php_info_print_table_row(2, "Persistent Sessions", buf);

  php_info_print_table_end();

  DISPLAY_INI_ENTRIES();
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
throw_invalid_argument(zval* object,
                       const char* object_name,
                       const char* expected_type TSRMLS_DC)
{
  if (Z_TYPE_P(object) == IS_OBJECT) {
#if ZEND_MODULE_API_NO >= 20100525
    const char* cls_name = NULL;
#else
    char* cls_name = NULL;
#endif
    zend_uint cls_len;

    Z_OBJ_HANDLER_P(object, get_class_name)(object, &cls_name, &cls_len, 0 TSRMLS_CC);
    if (cls_name) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "%s must be %s, an instance of %.*s given",
                              object_name, expected_type, cls_len, cls_name);
      efree((void*) cls_name);
    } else {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "%s must be %s, an instance of Unknown Class given",
                              object_name, expected_type);
    }
  } else {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "%s must be %s, '%Z' given",
                            object_name, expected_type, object);
  }
}
