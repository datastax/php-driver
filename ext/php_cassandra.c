#include "php_cassandra.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "util/bytes.h"
#include "util/math.h"
#include "util/collections.h"
#include "types/collection.h"
#include "types/map.h"
#include "types/set.h"

extern zend_class_entry* cassandra_ce_RuntimeException;
extern zend_class_entry* cassandra_ce_TimeoutException;
extern zend_class_entry* cassandra_ce_LogicException;
extern zend_class_entry* cassandra_ce_InvalidArgumentException;
extern zend_class_entry* cassandra_ce_ServerException;

extern zend_class_entry* cassandra_ce_OverloadedException;
extern zend_class_entry* cassandra_ce_IsBootstrappingException;

extern zend_class_entry* cassandra_ce_TruncateException;
extern zend_class_entry* cassandra_ce_WriteTimeoutException;
extern zend_class_entry* cassandra_ce_ReadTimeoutException;
extern zend_class_entry* cassandra_ce_TruncateException;
extern zend_class_entry* cassandra_ce_UnavailableException;

extern zend_class_entry* cassandra_ce_InvalidSyntaxException;
extern zend_class_entry* cassandra_ce_UnauthorizedException;
extern zend_class_entry* cassandra_ce_InvalidQueryException;
extern zend_class_entry* cassandra_ce_ConfigurationException;
extern zend_class_entry* cassandra_ce_AlreadyExistsException;
extern zend_class_entry* cassandra_ce_UnpreparedException;

extern zend_class_entry* cassandra_ce_ProtocolException;
extern zend_class_entry* cassandra_ce_AuthenticationException;

extern zend_class_entry* cassandra_ce_Bigint;
extern zend_class_entry* cassandra_ce_Blob;
extern zend_class_entry* cassandra_ce_Decimal;
extern zend_class_entry* cassandra_ce_Float;
extern zend_class_entry* cassandra_ce_Inet;
extern zend_class_entry* cassandra_ce_Timestamp;
extern zend_class_entry* cassandra_ce_Uuid;
extern zend_class_entry* cassandra_ce_Timeuuid;
extern zend_class_entry* cassandra_ce_Varint;

extern zend_class_entry* cassandra_ce_Set;
extern zend_class_entry* cassandra_ce_Map;
extern zend_class_entry* cassandra_ce_Collection;

static zend_class_entry*
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
    return cassandra_ce_InvalidArgumentException;
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
    return cassandra_ce_RuntimeException;
  case CASS_ERROR_LIB_REQUEST_TIMED_OUT:
    return cassandra_ce_TimeoutException;
  case CASS_ERROR_LIB_CALLBACK_ALREADY_SET:
  case CASS_ERROR_LIB_NOT_IMPLEMENTED:
    return cassandra_ce_LogicException;
  case CASS_ERROR_SERVER_SERVER_ERROR:
    return cassandra_ce_ServerException;
  case CASS_ERROR_SERVER_PROTOCOL_ERROR:
    return cassandra_ce_ProtocolException;
  case CASS_ERROR_SERVER_BAD_CREDENTIALS:
    return cassandra_ce_AuthenticationException;
  case CASS_ERROR_SERVER_UNAVAILABLE:
    return cassandra_ce_UnavailableException;
  case CASS_ERROR_SERVER_OVERLOADED:
    return cassandra_ce_OverloadedException;
  case CASS_ERROR_SERVER_IS_BOOTSTRAPPING:
    return cassandra_ce_IsBootstrappingException;
  case CASS_ERROR_SERVER_TRUNCATE_ERROR:
    return cassandra_ce_TruncateException;
  case CASS_ERROR_SERVER_WRITE_TIMEOUT:
    return cassandra_ce_WriteTimeoutException;
  case CASS_ERROR_SERVER_READ_TIMEOUT:
    return cassandra_ce_ReadTimeoutException;
  case CASS_ERROR_SERVER_SYNTAX_ERROR:
    return cassandra_ce_InvalidSyntaxException;
  case CASS_ERROR_SERVER_UNAUTHORIZED:
    return cassandra_ce_UnauthorizedException;
  case CASS_ERROR_SERVER_INVALID_QUERY:
    return cassandra_ce_InvalidQueryException;
  case CASS_ERROR_SERVER_CONFIG_ERROR:
    return cassandra_ce_ConfigurationException;
  case CASS_ERROR_SERVER_ALREADY_EXISTS:
    return cassandra_ce_AlreadyExistsException;
  case CASS_ERROR_SERVER_UNPREPARED:
    return cassandra_ce_UnpreparedException;
  default:
    return cassandra_ce_RuntimeException;
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

const zend_function_entry cassandra_functions[] = {
  /* Log */
  PHP_FE(cassandra_set_log_level, NULL)
  /* Util */
  PHP_FE(cassanrda_rows_from_result, NULL)
  /* CassCluster */
  PHP_FE(cassandra_cluster_new, NULL)
  PHP_FE(cassandra_cluster_free, NULL)
  PHP_FE(cassandra_cluster_set_load_balance_round_robin, NULL)
  PHP_FE(cassandra_cluster_set_load_balance_dc_aware, NULL)
  PHP_FE(cassandra_cluster_set_token_aware_routing, NULL)
  PHP_FE(cassandra_cluster_set_credentials, NULL)
  PHP_FE(cassandra_cluster_set_contact_points, NULL)
  PHP_FE(cassandra_cluster_set_ssl, NULL)
  /* CassSsl */
  PHP_FE(cassandra_ssl_new, NULL)
  PHP_FE(cassandra_ssl_free, NULL)
  PHP_FE(cassandra_ssl_add_trusted_cert, NULL)
  PHP_FE(cassandra_ssl_set_cert, NULL)
  PHP_FE(cassandra_ssl_set_private_key, NULL)
  PHP_FE(cassandra_ssl_set_verify_flags, NULL)
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
  STANDARD_MODULE_PROPERTIES
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
static void
php_cassandra_cluster_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassCluster* cluster = (CassCluster*) rsrc->ptr;

  if (cluster) {
    cass_cluster_free(cluster);
    rsrc->ptr = NULL;
  }
}

static int le_cassandra_ssl_res;
static void
php_cassandra_ssl_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassSsl* ssl = (CassSsl*) rsrc->ptr;

  if (ssl) {
    cass_ssl_free(ssl);
    rsrc->ptr = NULL;
  }
}

static int le_cassandra_session_res;
static void
php_cassandra_session_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassSession* session = (CassSession*) rsrc->ptr;

  if (session) {
    cass_session_free(session);
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

static void
php_cassandra_globals_ctor(zend_cassandra_globals* cassandra_globals TSRMLS_DC)
{
  cassandra_globals->uuid_gen  = cass_uuid_gen_new();
  cassandra_globals->log_level = CASS_LOG_ERROR;
  cass_log_set_callback(php_cassandra_log, NULL);
}

static void
php_cassandra_globals_dtor(zend_cassandra_globals* cassandra_globals TSRMLS_DC)
{
  cass_log_cleanup();
  cass_uuid_gen_free(cassandra_globals->uuid_gen);
  cassandra_globals->uuid_gen = NULL;
}

PHP_MINIT_FUNCTION(cassandra)
{
  // REGISTER_INI_ENTRIES();
#ifdef ZTS
  ts_allocate_id(
    &cassandra_globals_id,
    sizeof(zend_cassandra_globals),
    (ts_allocate_ctor) php_cassandra_globals_ctor,
    (ts_allocate_dtor) php_cassandra_globals_dtor
  );
#else
  php_cassandra_globals_ctor(&cassandra_globals TSRMLS_CC);
#endif

  le_cassandra_cluster_res = zend_register_list_destructors_ex(
    php_cassandra_cluster_dtor,
    NULL,
    PHP_CASSANDRA_CLUSTER_RES_NAME,
    module_number
  );
  le_cassandra_ssl_res = zend_register_list_destructors_ex(
    php_cassandra_ssl_dtor,
    NULL,
    PHP_CASSANDRA_SSL_RES_NAME,
    module_number
  );
  le_cassandra_session_res = zend_register_list_destructors_ex(
    php_cassandra_session_dtor,
    NULL,
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

  cassandra_define_CassandraException(TSRMLS_C);
  cassandra_define_CassandraInvalidArgumentException(TSRMLS_C);
  cassandra_define_CassandraDomainException(TSRMLS_C);
  cassandra_define_CassandraRuntimeException(TSRMLS_C);
  cassandra_define_CassandraTimeoutException(TSRMLS_C);
  cassandra_define_CassandraLogicException(TSRMLS_C);
  cassandra_define_CassandraServerException(TSRMLS_C);

  cassandra_define_CassandraBigint(TSRMLS_C);
  cassandra_define_CassandraBlob(TSRMLS_C);
  cassandra_define_CassandraDecimal(TSRMLS_C);
  cassandra_define_CassandraFloat(TSRMLS_C);
  cassandra_define_CassandraInet(TSRMLS_C);
  cassandra_define_CassandraTimestamp(TSRMLS_C);
  cassandra_define_CassandraUuidInterface(TSRMLS_C);
  cassandra_define_CassandraTimeuuid(TSRMLS_C);
  cassandra_define_CassandraUuid(TSRMLS_C);
  cassandra_define_CassandraVarint(TSRMLS_C);

  cassandra_define_CassandraSet(TSRMLS_C);
  cassandra_define_CassandraMap(TSRMLS_C);
  cassandra_define_CassandraCollection(TSRMLS_C);

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cassandra)
{
  // UNREGISTER_INI_ENTRIES();
#ifndef ZTS
  php_cassandra_globals_dtor(&cassandra_globals TSRMLS_CC);
#endif

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
  php_info_print_table_start( );
  php_info_print_table_header(2, "Cassandra support", "enabled");
  php_info_print_table_end( );
}

static zval* php_cassandra_value(const CassValue* value, CassValueType type);

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
    break;
  default:
    RETURN_FALSE;
  }

  CASSANDRA_G(log_level) = level;

  RETURN_TRUE;
}

PHP_FUNCTION(cassanrda_rows_from_result)
{
  CassResult* result;
  zval* result_resource;
  zval* row_value;
  CassIterator* iterator;
  const CassRow* row;
  cass_size_t columns_count;
  CassString column_name;
  CassValueType column_type;
  const CassValue* column_value;
  CassString output;
  zval* php_value;
  int i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(result, CassResult*, &result_resource, -1,
    PHP_CASSANDRA_RESULT_RES_NAME, le_cassandra_result_res);

  array_init(return_value);

  columns_count = cass_result_column_count(result);
  iterator      = cass_iterator_from_result(result);

  while (cass_iterator_next(iterator)) {
    MAKE_STD_ZVAL(row_value);
    array_init(row_value);
    row = cass_iterator_get_row(iterator);

    for (i = 0; i < columns_count; i++) {
      column_name  = cass_result_column_name(result, i);
      column_type  = cass_result_column_type(result, i);
      column_value = cass_row_get_column(row, i);
      php_value    = php_cassandra_value(column_value, column_type);

      add_assoc_zval_ex(row_value, column_name.data, column_name.length + 1, php_value);
    }

    add_next_index_zval(return_value, row_value);
  }

  cass_iterator_free(iterator);
}

PHP_FUNCTION(cassandra_cluster_new)
{
  CassCluster* cluster;
  cluster = cass_cluster_new();

  ZEND_REGISTER_RESOURCE(
    return_value,
    cluster,
    le_cassandra_cluster_res
  );
}

PHP_FUNCTION(cassandra_cluster_free)
{
  CassCluster* cluster;
  zval* cluster_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &cluster_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  zend_list_delete(Z_RESVAL_P(cluster_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_cluster_set_load_balance_round_robin)
{
  CassCluster* cluster;
  zval* cluster_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &cluster_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  cass_cluster_set_load_balance_round_robin(cluster);

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_cluster_set_load_balance_dc_aware)
{
  CassCluster* cluster;
  zval* cluster_resource;
  char* local_dc;
  int local_dc_len;
  char *name;
  long used_hosts_per_remote_dc;
  zend_bool allow_remote_dcs_for_local_cl;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rslb", &cluster_resource, &local_dc, &local_dc_len, &used_hosts_per_remote_dc, &allow_remote_dcs_for_local_cl) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  name = strndup(local_dc, local_dc_len);
  CassError rc = cass_cluster_set_load_balance_dc_aware(cluster, name, used_hosts_per_remote_dc, allow_remote_dcs_for_local_cl);
  free(name);
  CHECK_RESULT(rc);
}

PHP_FUNCTION(cassandra_cluster_set_token_aware_routing)
{
  CassCluster* cluster;
  zval* cluster_resource;
  zend_bool enabled;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rb", &cluster_resource, &enabled) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  cass_cluster_set_token_aware_routing(cluster, enabled);

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_cluster_set_credentials)
{
  CassCluster* cluster;
  zval* cluster_resource;
  char* username;
  int username_len;
  char* password;
  int password_len;
  char* username_dl;
  char* password_dl;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &cluster_resource, &username, &username_len, &password, &password_len) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  username_dl = strndup(username, username_len);
  password_dl = strndup(password, password_len);
  cass_cluster_set_credentials(cluster, username_dl, password_dl);
  free(username_dl);
  free(password_dl);

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_cluster_set_contact_points)
{
  CassCluster* cluster;
  char *contact_points;
  int contact_points_len;
  char* hosts;
  zval* cluster_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &cluster_resource, &contact_points, &contact_points_len) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  hosts = strndup(contact_points, contact_points_len);

  CassError rc = cass_cluster_set_contact_points(cluster, hosts);
  free(hosts);
  CHECK_RESULT(rc);
}

PHP_FUNCTION(cassandra_cluster_set_ssl)
{
  CassCluster* cluster;
  zval* cluster_resource;
  CassSsl* ssl;
  zval* ssl_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &cluster_resource, &ssl_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  ZEND_FETCH_RESOURCE(ssl, CassSsl*, &ssl_resource, -1,
    PHP_CASSANDRA_SSL_RES_NAME, le_cassandra_ssl_res);

  cass_cluster_set_ssl(cluster, ssl);

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_ssl_new)
{
  CassSsl* ssl;
  ssl = cass_ssl_new();

  ZEND_REGISTER_RESOURCE(
    return_value,
    ssl,
    le_cassandra_ssl_res
  );
}

PHP_FUNCTION(cassandra_ssl_free)
{
  CassSsl* ssl;
  zval* ssl_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &ssl_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(ssl, CassSsl*, &ssl_resource, -1,
    PHP_CASSANDRA_SSL_RES_NAME, le_cassandra_ssl_res);

  zend_list_delete(Z_RESVAL_P(ssl_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_ssl_add_trusted_cert)
{
  CassSsl* ssl;
  zval* ssl_resource;
  char* cert;
  int cert_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &ssl_resource, &cert, &cert_len) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(ssl, CassSsl*, &ssl_resource, -1,
    PHP_CASSANDRA_SSL_RES_NAME, le_cassandra_ssl_res);

  CHECK_RESULT(cass_ssl_add_trusted_cert(ssl, cass_string_init2(cert, cert_len)));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_ssl_set_cert)
{
  CassSsl* ssl;
  zval* ssl_resource;
  char* cert;
  int cert_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &ssl_resource, &cert, &cert_len) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(ssl, CassSsl*, &ssl_resource, -1,
    PHP_CASSANDRA_SSL_RES_NAME, le_cassandra_ssl_res);

  CHECK_RESULT(cass_ssl_set_cert(ssl, cass_string_init2(cert, cert_len)));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_ssl_set_private_key)
{
  CassSsl* ssl;
  zval* ssl_resource;
  char* key;
  int key_len;
  char* passphrase = NULL;
  int passphrase_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|s", &ssl_resource, &key, &key_len, &passphrase, &passphrase_len) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(ssl, CassSsl*, &ssl_resource, -1,
    PHP_CASSANDRA_SSL_RES_NAME, le_cassandra_ssl_res);

  CHECK_RESULT(cass_ssl_set_private_key(ssl, cass_string_init2(key, key_len), passphrase));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_ssl_set_verify_flags)
{
  CassSsl* ssl;
  zval* ssl_resource;
  long flags;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &ssl_resource, &flags) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(ssl, CassSsl*, &ssl_resource, -1,
    PHP_CASSANDRA_SSL_RES_NAME, le_cassandra_ssl_res);

  cass_ssl_set_verify_flags(ssl, (int) flags);

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
    zend_throw_exception_ex(cassandra_ce_TimeoutException, 0 TSRMLS_CC,
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
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC,
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
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC,
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
      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Float TSRMLS_CC)) {
        cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_float(statement, (cass_size_t) Z_LVAL_P(index), float_number->value));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Bigint TSRMLS_CC)) {
        cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_int64(statement, (cass_size_t) Z_LVAL_P(index), bigint->value));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Timestamp TSRMLS_CC)) {
        cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_int64(statement, (cass_size_t) Z_LVAL_P(index), timestamp->timestamp));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Blob TSRMLS_CC)) {
        cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_bytes(statement, (cass_size_t) Z_LVAL_P(index), cass_bytes_init(blob->data, blob->size)));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Varint TSRMLS_CC)) {
        cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
        cass_size_t size;
        cass_byte_t* data;
        data = (cass_byte_t*) export_twos_complement(varint->value, &size);
        CassError rc = cass_statement_bind_bytes(statement, (cass_size_t) Z_LVAL_P(index), cass_bytes_init(data, size));
        free(data);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Decimal TSRMLS_CC)) {
        cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
        cass_size_t size;
        cass_byte_t* data;
        data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
        CassError rc = cass_statement_bind_decimal(statement, (cass_size_t) Z_LVAL_P(index), cass_decimal_init(decimal->scale, cass_bytes_init(data, size)));
        free(data);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Uuid TSRMLS_CC)) {
        cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_uuid(statement, (cass_size_t) Z_LVAL_P(index), uuid->uuid));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Inet TSRMLS_CC)) {
        cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_inet(statement, (cass_size_t) Z_LVAL_P(index), inet->inet));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Set TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_set* set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_set(set, &collection))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection(statement, (cass_size_t) Z_LVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Map TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_map* map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_map(map, &collection))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection(statement, (cass_size_t) Z_LVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Collection TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_collection* coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_collection(coll, &collection))
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
      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Float TSRMLS_CC)) {
        cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_float_by_name(statement, Z_STRVAL_P(index), float_number->value));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Bigint TSRMLS_CC)) {
        cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_int64_by_name(statement, Z_STRVAL_P(index), bigint->value));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Timestamp TSRMLS_CC)) {
        cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_int64_by_name(statement, Z_STRVAL_P(index), timestamp->timestamp));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Blob TSRMLS_CC)) {
        cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_bytes_by_name(statement, Z_STRVAL_P(index), cass_bytes_init(blob->data, blob->size)));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Varint TSRMLS_CC)) {
        cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
        cass_size_t size;
        cass_byte_t* data;
        data = (cass_byte_t*) export_twos_complement(varint->value, &size);
        CassError rc = cass_statement_bind_bytes_by_name(statement, Z_STRVAL_P(index), cass_bytes_init(data, size));
        free(data);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Decimal TSRMLS_CC)) {
        cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
        cass_size_t size;
        cass_byte_t* data;
        data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
        CassError rc = cass_statement_bind_decimal_by_name(statement, Z_STRVAL_P(index), cass_decimal_init(decimal->scale, cass_bytes_init(data, size)));
        free(data);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Uuid TSRMLS_CC)) {
        cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_uuid_by_name(statement, Z_STRVAL_P(index), uuid->uuid));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Inet TSRMLS_CC)) {
        cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
        CHECK_RESULT(cass_statement_bind_inet_by_name(statement, Z_STRVAL_P(index), inet->inet));
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Set TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_set* set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_set(set, &collection))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection_by_name(statement, Z_STRVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Map TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_map* map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_map(map, &collection))
          RETURN_FALSE;

        CassError rc = cass_statement_bind_collection_by_name(statement, Z_STRVAL_P(index), collection);
        cass_collection_free(collection);
        CHECK_RESULT(rc);
        return;
      }

      if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Collection TSRMLS_CC)) {
        CassCollection* collection;
        cassandra_collection* coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
        if (!php_cassandra_collection_from_collection(coll, &collection))
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
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC,
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

static zval*
php_cassandra_value(const CassValue* value, CassValueType type)
{
  zval* return_value;
  CassError rc;
  CassString v_string;
  CassBytes v_bytes;
  CassUuid v_uuid;
  CassInet v_inet;
  CassDecimal v_decimal;
  cass_int64_t v_int_64;
  cass_int32_t v_int_32;
  cass_bool_t v_boolean;
  cass_double_t v_double;
  cass_float_t v_float;
  cassandra_uuid* uuid;
  CassIterator* iterator;
  MAKE_STD_ZVAL(return_value);
  char* string;
  int string_len;

  if (cass_value_is_null(value))
    return return_value;

  switch (type) {
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_VARCHAR:
    rc = cass_value_get_string(value, &v_string);
    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }
    RETVAL_STRINGL(v_string.data, v_string.length, 1);
    break;
  case CASS_VALUE_TYPE_INT:
    rc = cass_value_get_int32(value, &v_int_32);
    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }
    RETVAL_LONG(v_int_32);
    break;
  case CASS_VALUE_TYPE_COUNTER:
  case CASS_VALUE_TYPE_BIGINT:
    object_init_ex(return_value, cassandra_ce_Bigint);
    cassandra_bigint* bigint_number = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
    rc = cass_value_get_int64(value, &bigint_number->value);
    if (rc != CASS_OK) {
      zval_dtor(return_value);
      RETVAL_NULL();
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
    }
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    object_init_ex(return_value, cassandra_ce_Timestamp);
    cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(return_value TSRMLS_CC);
    rc = cass_value_get_int64(value, &timestamp->timestamp);
    if (rc != CASS_OK) {
      zval_dtor(return_value);
      RETVAL_NULL();
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
    }
    break;
  case CASS_VALUE_TYPE_BLOB:
    object_init_ex(return_value, cassandra_ce_Blob);
    cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(return_value TSRMLS_CC);
    rc = cass_value_get_bytes(value, &v_bytes);
    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }
    blob->data = emalloc((v_bytes.size) * sizeof(cass_byte_t));
    blob->size = v_bytes.size;
    memcpy(blob->data, v_bytes.data, v_bytes.size);
    break;
  case CASS_VALUE_TYPE_VARINT:
    rc = cass_value_get_bytes(value, &v_bytes);
    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }
    object_init_ex(return_value, cassandra_ce_Varint);
    cassandra_varint* varint_number = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);
    import_twos_complement((cass_byte_t*) v_bytes.data, v_bytes.size, &varint_number->value);
    break;
  case CASS_VALUE_TYPE_UUID:
    object_init_ex(return_value, cassandra_ce_Uuid);
    uuid = (cassandra_uuid*) zend_object_store_get_object(return_value TSRMLS_CC);

    rc = cass_value_get_uuid(value, &uuid->uuid);
    if (rc != CASS_OK) {
      zval_dtor(return_value);
      RETVAL_NULL();
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
    }
    break;
  case CASS_VALUE_TYPE_TIMEUUID:
    object_init_ex(return_value, cassandra_ce_Timeuuid);
    uuid = (cassandra_uuid*) zend_object_store_get_object(return_value TSRMLS_CC);

    rc = cass_value_get_uuid(value, &uuid->uuid);
    if (rc != CASS_OK) {
      zval_dtor(return_value);
      RETVAL_NULL();
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
    }
    break;
  case CASS_VALUE_TYPE_BOOLEAN:
    rc = cass_value_get_bool(value, &v_boolean);

    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }

    if (v_boolean)
      RETVAL_TRUE;
    else
      RETVAL_FALSE;

    break;
  case CASS_VALUE_TYPE_INET:
    object_init_ex(return_value, cassandra_ce_Inet);
    cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(return_value TSRMLS_CC);

    rc = cass_value_get_inet(value, &inet->inet);
    if (rc != CASS_OK) {
      zval_dtor(return_value);
      RETVAL_NULL();
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
    }
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    rc = cass_value_get_decimal(value, &v_decimal);
    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }
    object_init_ex(return_value, cassandra_ce_Decimal);
    cassandra_decimal* decimal_number = (cassandra_decimal*) zend_object_store_get_object(return_value TSRMLS_CC);
    import_twos_complement((cass_byte_t*) v_decimal.varint.data, v_decimal.varint.size, &decimal_number->value);
    decimal_number->scale = v_decimal.scale;
    break;
  case CASS_VALUE_TYPE_DOUBLE:
    rc = cass_value_get_double(value, &v_double);

    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }

    RETVAL_DOUBLE(v_double);
    break;
  case CASS_VALUE_TYPE_FLOAT:
    object_init_ex(return_value, cassandra_ce_Float);
    cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
    rc = cass_value_get_float(value, &float_number->value);
    if (rc != CASS_OK) {
      zval_dtor(return_value);
      RETVAL_NULL();
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
    }
    break;
  case CASS_VALUE_TYPE_LIST:
    object_init_ex(return_value, cassandra_ce_Collection);
    cassandra_collection* collection = (cassandra_collection*) zend_object_store_get_object(return_value TSRMLS_CC);
    collection->type = cass_value_primary_sub_type(value);

    iterator = cass_iterator_from_collection(value);

    while (cass_iterator_next(iterator)) {
      php_cassandra_collection_add(collection, php_cassandra_value(cass_iterator_get_value(iterator), collection->type) TSRMLS_CC);
    }

    cass_iterator_free(iterator);
    break;
  case CASS_VALUE_TYPE_MAP:
    object_init_ex(return_value, cassandra_ce_Map);
    cassandra_map* map = (cassandra_map*) zend_object_store_get_object(return_value TSRMLS_CC);
    map->key_type = cass_value_primary_sub_type(value);
    map->value_type = cass_value_secondary_sub_type(value);

    iterator = cass_iterator_from_map(value);

    while (cass_iterator_next(iterator)) {
      zval* k = php_cassandra_value(cass_iterator_get_map_key(iterator), map->key_type);
      zval* v = php_cassandra_value(cass_iterator_get_map_value(iterator), map->value_type);

      php_cassandra_map_set(map, k, v TSRMLS_CC);
    }

    cass_iterator_free(iterator);
    break;
  case CASS_VALUE_TYPE_SET:
    object_init_ex(return_value, cassandra_ce_Set);
    cassandra_set* set = (cassandra_set*) zend_object_store_get_object(return_value TSRMLS_CC);
    set->type = cass_value_primary_sub_type(value);

    iterator = cass_iterator_from_collection(value);

    while (cass_iterator_next(iterator)) {
      php_cassandra_set_add(set, php_cassandra_value(cass_iterator_get_value(iterator), set->type) TSRMLS_CC);
    }

    cass_iterator_free(iterator);
    break;
  default:
    RETVAL_NULL();
  }

  return return_value;
}
