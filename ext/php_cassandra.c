#include "php_cassandra.h"
#include <php_ini.h>
#include <ext/standard/info.h>
#include "util/bytes.h"
#include "util/math.h"
#include "util/collections.h"
#include "src/Cassandra/Types/Collection.h"
#include "src/Cassandra/Types/Map.h"
#include "src/Cassandra/Types/Set.h"

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

#define CHECK_RESULT(rc) \
  ({ \
    if (rc != CASS_OK) {\
      zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC, \
        "%s", cass_error_desc(rc)); \
      return; \
    } \
  })

ZEND_DECLARE_MODULE_GLOBALS(cassandra)
static PHP_GINIT_FUNCTION(cassandra);
static PHP_GSHUTDOWN_FUNCTION(cassandra);

const zend_function_entry cassandra_functions[] = {
  /* Log */
  PHP_FE(cassandra_set_log_level, NULL)
  /* CassSession */
  PHP_FE(cassandra_session_new, NULL)
  PHP_FE(cassandra_session_free, NULL)
  PHP_FE(cassandra_session_connect, NULL)
  PHP_FE(cassandra_session_connect_keyspace, NULL)
  PHP_FE(cassandra_session_execute, NULL)
  PHP_FE(cassandra_session_prepare, NULL)
  PHP_FE(cassandra_session_execute_batch, NULL)
  PHP_FE(cassandra_session_close, NULL)
  /* CassFuture */
  PHP_FE(cassandra_future_free, NULL)
  PHP_FE(cassandra_future_wait, NULL)
  PHP_FE(cassandra_future_wait_timed, NULL)
  PHP_FE(cassandra_future_get_result, NULL)
  PHP_FE(cassandra_future_get_prepared, NULL)
  /* CassResult */
  PHP_FE(cassandra_result_free, NULL)
  PHP_FE(cassandra_result_row_count, NULL)
  PHP_FE(cassandra_result_has_more_pages, NULL)
  /* CassStatement */
  PHP_FE(cassandra_statement_new, NULL)
  PHP_FE(cassandra_statement_free, NULL)
  PHP_FE(cassandra_statement_bind, NULL)
  PHP_FE(cassandra_statement_set_consistency, NULL)
  PHP_FE(cassandra_statement_set_paging_size, NULL)
  PHP_FE(cassandra_statement_set_serial_consistency, NULL)
  PHP_FE(cassandra_statement_set_paging_state, NULL)
  /* CassPrepared */
  PHP_FE(cassandra_prepared_free, NULL)
  PHP_FE(cassandra_prepared_bind, NULL)
  /* CassBatch */
  PHP_FE(cassandra_batch_new, NULL)
  PHP_FE(cassandra_batch_free, NULL)
  PHP_FE(cassandra_batch_set_consistency, NULL)
  PHP_FE(cassandra_batch_add_statement, NULL)
  PHP_FE_END /* Must be the last line in cassandra_functions[] */
};

#if ZEND_MODULE_API_NO >= 20050617
static zend_module_dep php_cassandra_deps[] = {
  ZEND_MOD_REQUIRED("spl")
#ifdef ZEND_MOD_END
  ZEND_MOD_END
#else
  {NULL,NULL,NULL}
#endif
};
#endif

zend_module_entry cassandra_module_entry = {
#if ZEND_MODULE_API_NO >= 20050617
  STANDARD_MODULE_HEADER_EX, NULL,
  php_cassandra_deps,
#elif ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_CASSANDRA_EXTNAME,
  cassandra_functions,      /* Functions */
  PHP_MINIT(cassandra),     /* MINIT */
  PHP_MSHUTDOWN(cassandra), /* MSHUTDOWN */
  PHP_RINIT(cassandra),     /* RINIT */
  PHP_RSHUTDOWN(cassandra), /* RSHUTDOWN */
  PHP_MINFO(cassandra),     /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
  PHP_CASSANDRA_EXTVER,
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
php_cassandra_log(const CassLogMessage* message, void *data)
{
  fprintf(stderr, "php-driver | [%s] %s (%s:%d)\n",
    cass_log_level_string(message->severity), message->message,
    message->file, message->line
  );
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

static int le_cassandra_future_res;
static void
php_cassandra_future_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassFuture* future = (CassFuture*) rsrc->ptr;

  if (future) {
    cass_future_free(future);
    rsrc->ptr = NULL;
  }
}

static int le_cassandra_result_res;
static void
php_cassandra_result_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassResult* result = (CassResult*) rsrc->ptr;

  if (result) {
    cass_result_free(result);
    rsrc->ptr = NULL;
  }
}

static int le_cassandra_prepared_res;
static void
php_cassandra_prepared_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassPrepared* prepared = (CassPrepared*) rsrc->ptr;

  if (prepared) {
    cass_prepared_free(prepared);
    rsrc->ptr = NULL;
  }
}

static int le_cassandra_statement_res;
static void
php_cassandra_statement_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassStatement* statement = (CassStatement*) rsrc->ptr;

  if (statement) {
    cass_statement_free(statement);
    rsrc->ptr = NULL;
  }
}

static int le_cassandra_batch_res;
static void
php_cassandra_batch_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassBatch* batch = (CassBatch*) rsrc->ptr;

  if (batch) {
    cass_batch_free(batch);
    rsrc->ptr = NULL;
  }
}

static PHP_GINIT_FUNCTION(cassandra)
{
  cassandra_globals->uuid_gen            = cass_uuid_gen_new();
  cassandra_globals->log_level           = CASS_LOG_ERROR;
  cassandra_globals->persistent_clusters = 0;
  cassandra_globals->persistent_sessions = 0;
  cass_log_set_callback(php_cassandra_log, NULL);
}

static PHP_GSHUTDOWN_FUNCTION(cassandra)
{
  cass_log_cleanup();
  cass_uuid_gen_free(cassandra_globals->uuid_gen);
}

PHP_MINIT_FUNCTION(cassandra)
{
  // REGISTER_INI_ENTRIES();
  le_cassandra_cluster_res = zend_register_list_destructors_ex(
    NULL,
    php_cassandra_cluster_dtor,
    PHP_CASSANDRA_CLUSTER_RES_NAME,
    module_number
  );
  le_cassandra_session_res = zend_register_list_destructors_ex(
    NULL,
    php_cassandra_session_dtor,
    PHP_CASSANDRA_SESSION_RES_NAME,
    module_number
  );
  le_cassandra_future_res = zend_register_list_destructors_ex(
    php_cassandra_future_dtor,
    NULL,
    PHP_CASSANDRA_FUTURE_RES_NAME,
    module_number
  );
  le_cassandra_result_res = zend_register_list_destructors_ex(
    php_cassandra_result_dtor,
    NULL,
    PHP_CASSANDRA_RESULT_RES_NAME,
    module_number
  );
  le_cassandra_prepared_res = zend_register_list_destructors_ex(
    php_cassandra_prepared_dtor,
    NULL,
    PHP_CASSANDRA_PREPARED_RES_NAME,
    module_number
  );
  le_cassandra_statement_res = zend_register_list_destructors_ex(
    php_cassandra_statement_dtor,
    NULL,
    PHP_CASSANDRA_STATEMENT_RES_NAME,
    module_number
  );
  le_cassandra_batch_res = zend_register_list_destructors_ex(
    php_cassandra_batch_dtor,
    NULL,
    PHP_CASSANDRA_BATCH_RES_NAME,
    module_number
  );

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
  cassandra_define_DivideByZeroException(TSRMLS_C);
  cassandra_define_RangeException(TSRMLS_C);

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

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cassandra)
{
  // UNREGISTER_INI_ENTRIES();
  return SUCCESS;
}

PHP_RINIT_FUNCTION(cassandra)
{
  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(cassandra)
{
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
}

PHP_FUNCTION(cassandra_set_log_level)
{
  long level;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &level) == FAILURE) {
    RETURN_FALSE;
  }

  switch (level) {
  case CASS_LOG_DISABLED:
  case CASS_LOG_CRITICAL:
  case CASS_LOG_ERROR:
  case CASS_LOG_WARN:
  case CASS_LOG_INFO:
  case CASS_LOG_DEBUG:
  case CASS_LOG_TRACE:
    cass_log_set_level(level);
    break;
  default:
    RETURN_FALSE;
  }

  CASSANDRA_G(log_level) = level;

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_session_new)
{
  CassSession* session;
  session = cass_session_new();

  ZEND_REGISTER_RESOURCE(
    return_value,
    session,
    le_cassandra_session_res
  );
}

PHP_FUNCTION(cassandra_session_free)
{
  CassSession* session;
  zval* session_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &session_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(session, CassSession*, &session_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_session_res);

  zend_list_delete(Z_RESVAL_P(session_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_session_connect)
{
  CassCluster* cluster;
  CassSession* session;
  CassFuture*  future;
  zval* session_resource;
  zval* cluster_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &session_resource, &cluster_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(session, CassSession*, &session_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_session_res);

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  future = cass_session_connect(session, cluster);

  ZEND_REGISTER_RESOURCE(
    return_value,
    future,
    le_cassandra_future_res
  );
}

PHP_FUNCTION(cassandra_session_connect_keyspace)
{
  CassCluster* cluster;
  CassSession* session;
  CassFuture*  future;
  zval* session_resource;
  zval* cluster_resource;
  char* keyspace;
  int keyspace_len;
  char* ks;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrs", &session_resource, &cluster_resource, &keyspace, &keyspace_len) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(session, CassSession*, &session_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_session_res);

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  ks     = strndup(keyspace, keyspace_len);
  future = cass_session_connect_keyspace(session, cluster, ks);

  ZEND_REGISTER_RESOURCE(
    return_value,
    future,
    le_cassandra_future_res
  );
  free(ks);
}

PHP_FUNCTION(cassandra_session_execute)
{
  CassSession* session;
  CassStatement* statement;
  CassFuture*  future;
  zval* session_resource;
  zval* statement_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &session_resource, &statement_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(session, CassSession*, &session_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_session_res);

  ZEND_FETCH_RESOURCE(statement, CassStatement*, &statement_resource, -1,
    PHP_CASSANDRA_STATEMENT_RES_NAME, le_cassandra_statement_res);

  future = cass_session_execute(session, statement);

  ZEND_REGISTER_RESOURCE(
    return_value,
    future,
    le_cassandra_future_res
  );
}

PHP_FUNCTION(cassandra_session_prepare)
{
  CassSession* session;
  CassStatement* statement;
  CassFuture*  future;
  zval* session_resource;
  char* cql;
  int cql_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &session_resource, &cql, &cql_len) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(session, CassSession*, &session_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_session_res);

  future = cass_session_prepare(session, cass_string_init2(cql, cql_len));

  ZEND_REGISTER_RESOURCE(
    return_value,
    future,
    le_cassandra_future_res
  );
}

PHP_FUNCTION(cassandra_session_execute_batch)
{
  CassSession* session;
  CassBatch* batch;
  CassFuture*  future;
  zval* session_resource;
  zval* batch_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &session_resource, &batch_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(session, CassSession*, &session_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_session_res);

  ZEND_FETCH_RESOURCE(batch, CassBatch*, &batch_resource, -1,
    PHP_CASSANDRA_BATCH_RES_NAME, le_cassandra_batch_res);

  future = cass_session_execute_batch(session, batch);

  ZEND_REGISTER_RESOURCE(
    return_value,
    future,
    le_cassandra_future_res
  );
}

PHP_FUNCTION(cassandra_session_close)
{
  CassSession* session;
  zval* session_resource;
  CassFuture*  future;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &session_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(session, CassSession*, &session_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_session_res);

  future = cass_session_close(session);

  ZEND_REGISTER_RESOURCE(
    return_value,
    future,
    le_cassandra_future_res
  );
}

PHP_FUNCTION(cassandra_future_free)
{
  CassFuture* future;
  zval* future_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &future_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_FUTURE_RES_NAME, le_cassandra_future_res);

  zend_list_delete(Z_RESVAL_P(future_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_future_wait)
{
  CassFuture* future;
  zval* future_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &future_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_FUTURE_RES_NAME, le_cassandra_future_res);

  cass_future_wait(future);

  CassError rc = cass_future_error_code(future);

  if (rc == CASS_OK)
    return;

  CassString message = cass_future_error_message(future);

  zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
    "%.*s", message.length, message.data);
}

PHP_FUNCTION(cassandra_future_wait_timed)
{
  CassFuture* future;
  zval* future_resource;
  double timeout;
  int t;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rd", &future_resource, &timeout) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_FUTURE_RES_NAME, le_cassandra_future_res);

  t = (int) ceil(timeout * 1000000);

  if (!cass_future_wait_timed(future, t)) {
    zend_throw_exception_ex(cassandra_timeout_exception_ce, 0 TSRMLS_CC,
      "Unable to resolve future within %d seconds", timeout);
    return;
  }

  CassError rc = cass_future_error_code(future);

  if (rc == CASS_OK)
    return;

  CassString message = cass_future_error_message(future);

  zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
    "%.*s", message.length, message.data);
}

PHP_FUNCTION(cassandra_future_get_result)
{
  CassFuture* future;
  zval* future_resource;
  const CassResult* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &future_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_FUTURE_RES_NAME, le_cassandra_future_res);

  result = cass_future_get_result(future);

  if (result == NULL) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
      "Invalid result future");
    return;
  }

  ZEND_REGISTER_RESOURCE(
    return_value,
    (void*) result,
    le_cassandra_result_res
  );
}

PHP_FUNCTION(cassandra_future_get_prepared)
{
  CassFuture* future;
  zval* future_resource;
  const CassPrepared* prepared;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &future_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_FUTURE_RES_NAME, le_cassandra_future_res);

  prepared = cass_future_get_prepared(future);

  if (prepared == NULL) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
      "Invalid prepared statement future");
    return;
  }

  ZEND_REGISTER_RESOURCE(
    return_value,
    (void*) prepared,
    le_cassandra_prepared_res
  );
}

PHP_FUNCTION(cassandra_result_free)
{
  CassResult* result;
  zval* result_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(result, CassResult*, &result_resource, -1,
    PHP_CASSANDRA_RESULT_RES_NAME, le_cassandra_result_res);

  zend_list_delete(Z_RESVAL_P(result_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_result_row_count)
{
  CassResult* result;
  zval* result_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(result, CassResult*, &result_resource, -1,
    PHP_CASSANDRA_RESULT_RES_NAME, le_cassandra_result_res);

  RETURN_LONG(cass_result_row_count(result));
}

PHP_FUNCTION(cassandra_result_has_more_pages)
{
  CassResult* result;
  zval* result_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(result, CassResult*, &result_resource, -1,
    PHP_CASSANDRA_RESULT_RES_NAME, le_cassandra_result_res);

  if (cass_result_has_more_pages(result))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_FUNCTION(cassandra_statement_new)
{
  CassStatement* statement;
  char* cql;
  int cql_len;
  long count;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &cql, &cql_len, &count) == FAILURE) {
    RETURN_FALSE;
  }

  statement = cass_statement_new(cass_string_init2(cql, cql_len), count);

  ZEND_REGISTER_RESOURCE(
    return_value,
    statement,
    le_cassandra_statement_res
  );
}

PHP_FUNCTION(cassandra_statement_free)
{
  CassStatement* statement;
  zval* statement_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &statement_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(statement, CassStatement*, &statement_resource, -1,
    PHP_CASSANDRA_STATEMENT_RES_NAME, le_cassandra_statement_res);

  zend_list_delete(Z_RESVAL_P(statement_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_statement_bind)
{
  CassStatement* statement;
  zval* statement_resource;
  zval* value;
  zval* index;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &statement_resource, &index, &value) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(statement, CassStatement*, &statement_resource, -1,
    PHP_CASSANDRA_STATEMENT_RES_NAME, le_cassandra_statement_res);

  if (Z_TYPE_P(index) == IS_LONG) {
    if (Z_TYPE_P(value) == IS_NULL)
      CHECK_RESULT(cass_statement_bind_null(statement, (cass_size_t) Z_LVAL_P(index)));

    if (Z_TYPE_P(value) == IS_STRING)
      CHECK_RESULT(cass_statement_bind_string(statement, (cass_size_t) Z_LVAL_P(index),
        cass_string_init2(Z_STRVAL_P(value), Z_STRLEN_P(value))));

    if (Z_TYPE_P(value) == IS_DOUBLE)
      CHECK_RESULT(cass_statement_bind_double(statement, (cass_size_t) Z_LVAL_P(index), Z_DVAL_P(value)));

    if (Z_TYPE_P(value) == IS_LONG)
      CHECK_RESULT(cass_statement_bind_int32(statement, (cass_size_t) Z_LVAL_P(index), Z_LVAL_P(value)));

    if (Z_TYPE_P(value) == IS_BOOL)
      CHECK_RESULT(cass_statement_bind_bool(statement, (cass_size_t) Z_LVAL_P(index), Z_BVAL_P(value)));

    if (Z_TYPE_P(value) == IS_OBJECT) {
      if (instanceof_function(Z_OBJCE_P(value), cassandra_float_ce TSRMLS_CC)) {
        cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_float(statement, (cass_size_t) Z_LVAL_P(index), float_number->value));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_bigint_ce TSRMLS_CC)) {
        cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_int64(statement, (cass_size_t) Z_LVAL_P(index), bigint->value));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_timestamp_ce TSRMLS_CC)) {
        cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_int64(statement, (cass_size_t) Z_LVAL_P(index), timestamp->timestamp));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_blob_ce TSRMLS_CC)) {
        cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_bytes(statement, (cass_size_t) Z_LVAL_P(index), cass_bytes_init(blob->data, blob->size)));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_varint_ce TSRMLS_CC)) {
        cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
        cass_size_t size;
        cass_byte_t* data;
        data = (cass_byte_t*) export_twos_complement(varint->value, &size);
        CassError rc = cass_statement_bind_bytes(statement, (cass_size_t) Z_LVAL_P(index), cass_bytes_init(data, size));
        free(data);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_decimal_ce TSRMLS_CC)) {
        cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
        cass_size_t size;
        cass_byte_t* data;
        data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
        CassError rc = cass_statement_bind_decimal(statement, (cass_size_t) Z_LVAL_P(index), cass_decimal_init(decimal->scale, cass_bytes_init(data, size)));
        free(data);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_uuid_ce TSRMLS_CC)) {
        cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_uuid(statement, (cass_size_t) Z_LVAL_P(index), uuid->uuid));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_inet_ce TSRMLS_CC)) {
        cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_inet(statement, (cass_size_t) Z_LVAL_P(index), inet->inet));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_set_ce TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_set* set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_set(set, &collection TSRMLS_CC))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection(statement, (cass_size_t) Z_LVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_map_ce TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_map* map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_map(map, &collection TSRMLS_CC))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection(statement, (cass_size_t) Z_LVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_collection_ce TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_collection* coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_collection(coll, &collection TSRMLS_CC))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection(statement, (cass_size_t) Z_LVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }
    }
  } else if (Z_TYPE_P(index) == IS_STRING) {
    if (Z_TYPE_P(value) == IS_NULL)
      CHECK_RESULT(cass_statement_bind_null_by_name(statement, Z_STRVAL_P(index)));

    if (Z_TYPE_P(value) == IS_STRING)
      CHECK_RESULT(cass_statement_bind_string_by_name(statement, Z_STRVAL_P(index),
        cass_string_init2(Z_STRVAL_P(value), Z_STRLEN_P(value))));

    if (Z_TYPE_P(value) == IS_DOUBLE)
      CHECK_RESULT(cass_statement_bind_double_by_name(statement, Z_STRVAL_P(index), Z_DVAL_P(value)));

    if (Z_TYPE_P(value) == IS_LONG)
      CHECK_RESULT(cass_statement_bind_int32_by_name(statement, Z_STRVAL_P(index), Z_LVAL_P(value)));

    if (Z_TYPE_P(value) == IS_BOOL)
      CHECK_RESULT(cass_statement_bind_bool_by_name(statement, Z_STRVAL_P(index), Z_BVAL_P(value)));

    if (Z_TYPE_P(value) == IS_OBJECT) {
      if (instanceof_function(Z_OBJCE_P(value), cassandra_float_ce TSRMLS_CC)) {
        cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_float_by_name(statement, Z_STRVAL_P(index), float_number->value));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_bigint_ce TSRMLS_CC)) {
        cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_int64_by_name(statement, Z_STRVAL_P(index), bigint->value));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_timestamp_ce TSRMLS_CC)) {
        cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_int64_by_name(statement, Z_STRVAL_P(index), timestamp->timestamp));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_blob_ce TSRMLS_CC)) {
        cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_bytes_by_name(statement, Z_STRVAL_P(index), cass_bytes_init(blob->data, blob->size)));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_varint_ce TSRMLS_CC)) {
        cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
        cass_size_t size;
        cass_byte_t* data;
        data = (cass_byte_t*) export_twos_complement(varint->value, &size);
        CassError rc = cass_statement_bind_bytes_by_name(statement, Z_STRVAL_P(index), cass_bytes_init(data, size));
        free(data);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_decimal_ce TSRMLS_CC)) {
        cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
        cass_size_t size;
        cass_byte_t* data;
        data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
        CassError rc = cass_statement_bind_decimal_by_name(statement, Z_STRVAL_P(index), cass_decimal_init(decimal->scale, cass_bytes_init(data, size)));
        free(data);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_uuid_ce TSRMLS_CC)) {
        cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_uuid_by_name(statement, Z_STRVAL_P(index), uuid->uuid));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_inet_ce TSRMLS_CC)) {
        cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_inet_by_name(statement, Z_STRVAL_P(index), inet->inet));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_set_ce TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_set* set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_set(set, &collection TSRMLS_CC))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection_by_name(statement, Z_STRVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_map_ce TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_map* map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_map(map, &collection TSRMLS_CC))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection_by_name(statement, Z_STRVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_collection_ce TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_collection* coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_collection(coll, &collection TSRMLS_CC))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection_by_name(statement, Z_STRVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }
    }
  }

  RETURN_FALSE;
}

PHP_FUNCTION(cassandra_statement_set_consistency)
{
  CassStatement* statement;
  zval* statement_resource;
  long consistency;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &statement_resource, &consistency) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(statement, CassStatement*, &statement_resource, -1,
    PHP_CASSANDRA_STATEMENT_RES_NAME, le_cassandra_statement_res);

  CHECK_RESULT(cass_statement_set_consistency(statement, consistency));
}

PHP_FUNCTION(cassandra_statement_set_paging_size)
{
  CassStatement* statement;
  zval* statement_resource;
  long page_size;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &statement_resource, &page_size) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(statement, CassStatement*, &statement_resource, -1,
    PHP_CASSANDRA_STATEMENT_RES_NAME, le_cassandra_statement_res);

  CHECK_RESULT(cass_statement_set_paging_size(statement, (int) page_size));
}

PHP_FUNCTION(cassandra_statement_set_serial_consistency)
{
  CassStatement* statement;
  zval* statement_resource;
  long consistency;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &statement_resource, &consistency) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(statement, CassStatement*, &statement_resource, -1,
    PHP_CASSANDRA_STATEMENT_RES_NAME, le_cassandra_statement_res);

  CHECK_RESULT(cass_statement_set_serial_consistency(statement, consistency));
}

PHP_FUNCTION(cassandra_statement_set_paging_state)
{
  CassStatement* statement;
  zval* statement_resource;
  CassResult* result;
  zval* result_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &statement_resource, &result_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(statement, CassStatement*, &statement_resource, -1,
    PHP_CASSANDRA_STATEMENT_RES_NAME, le_cassandra_statement_res);

  ZEND_FETCH_RESOURCE(result, CassResult*, &result_resource, -1,
    PHP_CASSANDRA_RESULT_RES_NAME, le_cassandra_result_res);

  CHECK_RESULT(cass_statement_set_paging_state(statement, result));
}

PHP_FUNCTION(cassandra_prepared_free)
{
  CassPrepared* prepared;
  zval* prepared_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &prepared_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(prepared, CassPrepared*, &prepared_resource, -1,
    PHP_CASSANDRA_PREPARED_RES_NAME, le_cassandra_prepared_res);

  zend_list_delete(Z_RESVAL_P(prepared_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_prepared_bind)
{
  CassPrepared* prepared;
  CassStatement* statement;
  zval* prepared_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &prepared_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(prepared, CassPrepared*, &prepared_resource, -1,
    PHP_CASSANDRA_PREPARED_RES_NAME, le_cassandra_prepared_res);

  statement = cass_prepared_bind(prepared);

  ZEND_REGISTER_RESOURCE(
    return_value,
    statement,
    le_cassandra_statement_res
  );
}

/* CassBatch */

PHP_FUNCTION(cassandra_batch_new)
{
  CassBatch* batch;
  long type;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &type) == FAILURE) {
    RETURN_FALSE;
  }

  if (type != CASS_BATCH_TYPE_LOGGED &&
      type != CASS_BATCH_TYPE_UNLOGGED &&
      type != CASS_BATCH_TYPE_COUNTER) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
      "Invalid batch type");
  }

  batch = cass_batch_new((CassBatchType) type);

  ZEND_REGISTER_RESOURCE(
    return_value,
    batch,
    le_cassandra_batch_res
  );
}

PHP_FUNCTION(cassandra_batch_free)
{
  CassBatch* batch;
  zval* batch_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &batch_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(batch, CassBatch*, &batch_resource, -1,
    PHP_CASSANDRA_BATCH_RES_NAME, le_cassandra_batch_res);

  zend_list_delete(Z_RESVAL_P(batch_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_batch_set_consistency)
{
  CassBatch* batch;
  zval* batch_resource;
  long consistency;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &batch_resource, &consistency) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(batch, CassBatch*, &batch_resource, -1,
    PHP_CASSANDRA_BATCH_RES_NAME, le_cassandra_batch_res);

  CHECK_RESULT(cass_batch_set_consistency(batch, consistency));
}

PHP_FUNCTION(cassandra_batch_add_statement)
{
  CassBatch* batch;
  zval* batch_resource;
  CassStatement* statement;
  zval* statement_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &batch_resource, &statement_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(batch, CassBatch*, &batch_resource, -1,
    PHP_CASSANDRA_BATCH_RES_NAME, le_cassandra_batch_res);

  ZEND_FETCH_RESOURCE(statement, CassStatement*, &statement_resource, -1,
    PHP_CASSANDRA_STATEMENT_RES_NAME, le_cassandra_statement_res);

  CHECK_RESULT(cass_batch_add_statement(batch, statement));
}

void throw_invalid_argument(zval* object,
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
      efree((void *)cls_name);
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
