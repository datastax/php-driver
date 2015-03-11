#include "php_cassandra.h"
#include "php_ini.h"
#include "ext/standard/info.h"

extern zend_class_entry *cassandra_ce_Bigint;
extern zend_class_entry *cassandra_ce_Blob;
extern zend_class_entry *cassandra_ce_Decimal;
extern zend_class_entry *cassandra_ce_Timestamp;
extern zend_class_entry *cassandra_ce_Varint;

ZEND_DECLARE_MODULE_GLOBALS(cassandra)

const zend_function_entry cassandra_functions[] = {
  /* Util */
  PHP_FE(cassandra_error_message, NULL)
  PHP_FE(cassanrda_rows_from_result, NULL)
  /* CassCluster */
  PHP_FE(cassandra_cluster_new, NULL)
  PHP_FE(cassandra_cluster_free, NULL)
  PHP_FE(cassandra_cluster_set_load_balance_round_robin, NULL)
  PHP_FE(cassandra_cluster_set_load_balance_dc_aware, NULL)
  PHP_FE(cassandra_cluster_set_token_aware_routing, NULL)
  PHP_FE(cassandra_cluster_set_credentials, NULL)
  PHP_FE(cassandra_cluster_set_contact_points, NULL)
  /* CassSession */
  PHP_FE(cassandra_session_new, NULL)
  PHP_FE(cassandra_session_free, NULL)
  PHP_FE(cassandra_session_connect, NULL)
  PHP_FE(cassandra_session_connect_keyspace, NULL)
  PHP_FE(cassandra_session_execute, NULL)
  /* CassFuture */
  PHP_FE(cassandra_future_free, NULL)
  PHP_FE(cassandra_future_wait, NULL)
  PHP_FE(cassandra_future_wait_timed, NULL)
  PHP_FE(cassandra_future_error_code, NULL)
  PHP_FE(cassandra_future_error_message, NULL)
  PHP_FE(cassandra_future_get_result, NULL)
  /* CassResult */
  PHP_FE(cassandra_result_free, NULL)
  PHP_FE(cassandra_result_row_count, NULL)
  /* CassStatement */
  PHP_FE(cassandra_statement_new, NULL)
  PHP_FE(cassandra_statement_free, NULL)
  PHP_FE_END /* Must be the last line in cassandra_functions[] */
};

#if ZEND_MODULE_API_NO >= 20050617
static zend_module_dep php_cassandra_deps[] = {
  ZEND_MOD_REQUIRED("spl")
  ZEND_MOD_REQUIRED("mbstring")
  ZEND_MOD_REQUIRED("bcmath")
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

PHP_MINIT_FUNCTION(cassandra)
{
  // REGISTER_INI_ENTRIES();
  le_cassandra_cluster_res = zend_register_list_destructors_ex(
    php_cassandra_cluster_dtor,
    NULL,
    PHP_CASSANDRA_CLUSTER_RES_NAME,
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
  le_cassandra_statement_res = zend_register_list_destructors_ex(
    php_cassandra_statement_dtor,
    NULL,
    PHP_CASSANDRA_STATEMENT_RES_NAME,
    module_number
  );

  cassandra_define_CassandraException(TSRMLS_C);
  cassandra_define_CassandraInvalidArgumentException(TSRMLS_C);

  cassandra_define_CassandraBigint(TSRMLS_C);
  cassandra_define_CassandraBlob(TSRMLS_C);
  cassandra_define_CassandraDecimal(TSRMLS_C);
  cassandra_define_CassandraTimestamp(TSRMLS_C);
  cassandra_define_CassandraVarint(TSRMLS_C);

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
  php_info_print_table_start( );
  php_info_print_table_header(2, "Cassandra support", "enabled");
  php_info_print_table_end( );
}

PHP_FUNCTION(cassandra_error_message)
{
  long code;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &code) == FAILURE) {
    RETURN_FALSE;
  }

  RETURN_STRING(cass_error_desc(code), 1);
}

static zval* php_cassandra_value(const CassValue* value, CassValueType type);

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
  RETVAL_LONG(cass_cluster_set_load_balance_dc_aware(cluster, name, used_hosts_per_remote_dc, allow_remote_dcs_for_local_cl));
  free(name);
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

  RETVAL_LONG(cass_cluster_set_contact_points(cluster, hosts));
  free(hosts);
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

  RETURN_TRUE;
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

  if (cass_future_wait_timed(future, t)) {
    RETURN_TRUE;
  }

  RETURN_FALSE;
}

PHP_FUNCTION(cassandra_future_error_code)
{
  CassFuture* future;
  zval* future_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &future_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_FUTURE_RES_NAME, le_cassandra_future_res);

  RETURN_LONG(cass_future_error_code(future));
}

PHP_FUNCTION(cassandra_future_error_message)
{
  CassFuture* future;
  CassString  message;
  zval* future_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &future_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_FUTURE_RES_NAME, le_cassandra_future_res);

  message = cass_future_error_message(future);
  RETURN_STRINGL(message.data, message.length, 1);
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
    RETURN_NULL();
  }

  ZEND_REGISTER_RESOURCE(
    return_value,
    (void*) result,
    le_cassandra_result_res
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
  MAKE_STD_ZVAL(return_value);
  char* string;
  int string_len;

  if (cass_value_is_null(value)) {
    RETVAL_NULL();
    return return_value;
  }

  switch (type) {
  case CASS_VALUE_TYPE_UNKNOWN:
  case CASS_VALUE_TYPE_CUSTOM:
    RETVAL_NULL();
    break;
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
    rc = cass_value_get_int64(value, &v_int_64);
    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
    }
    char *tmp_string;

#ifdef WIN32
    spprintf(&tmp_string, 0, "%I64d", v_int_64);
#else
    spprintf(&tmp_string, 0, "%lld", v_int_64);
#endif

    object_init_ex(return_value, cassandra_ce_Bigint);
    zend_update_property_string(cassandra_ce_Bigint, return_value, "value", strlen("value"), tmp_string TSRMLS_CC);

    efree(tmp_string);
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    rc = cass_value_get_int64(value, &v_int_64);
    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }

    object_init_ex(return_value, cassandra_ce_Timestamp);

    long sec  = (long) (v_int_64 / 1000);
    long usec = (long) ((v_int_64 - (sec * 1000)) * 1000);

    cassandra_timestamp* timestamp;

    zend_update_property_long(cassandra_ce_Timestamp, return_value, "seconds", strlen("seconds"), sec TSRMLS_CC);
    zend_update_property_long(cassandra_ce_Timestamp, return_value, "microseconds", strlen("microseconds"), usec TSRMLS_CC);

    timestamp = (cassandra_timestamp*) zend_object_store_get_object(return_value TSRMLS_CC);
    timestamp->timestamp = v_int_64;
    break;
  case CASS_VALUE_TYPE_BLOB:
    rc = cass_value_get_bytes(value, &v_bytes);
    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }

    object_init_ex(return_value, cassandra_ce_Blob);
    zend_update_property_stringl(cassandra_ce_Blob, return_value, "bytes", strlen("bytes"), (const char *) v_bytes.data, v_bytes.size TSRMLS_CC);
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
    mpz_import(varint_number->value, v_bytes.size, 1, sizeof(v_bytes.data[0]), 1, 0, v_bytes.data);

    php_cassandra_format_integer(varint_number->value, &string, &string_len);

    zend_update_property_stringl(cassandra_ce_Varint, return_value, "value", strlen("value"), string, string_len TSRMLS_CC);
    efree(string);
    break;
  case CASS_VALUE_TYPE_UUID:
    // TODO: implement Uuid
    RETVAL_NULL();
    break;
    // rc = cass_value_get_uuid(value, &v_uuid);
    // if (rc != CASS_OK) {
    //   php_error_docref(NULL TSRMLS_CC, E_WARNING,
    //     "Decoding error: %s", cass_error_desc(rc)
    //   );
    //   RETVAL_NULL();
    //   break;
    // }
  case CASS_VALUE_TYPE_TIMEUUID:
    // TODO: implement Timeuuid
    RETVAL_NULL();
    break;
    // rc = cass_value_get_uuid(value, &v_uuid);
    // if (rc != CASS_OK) {
    //   php_error_docref(NULL TSRMLS_CC, E_WARNING,
    //     "Decoding error: %s", cass_error_desc(rc)
    //   );
    //   RETVAL_NULL();
    //   break;
    // }
  case CASS_VALUE_TYPE_BOOLEAN:
    rc = cass_value_get_bool(value, &v_boolean);

    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }

    if (v_boolean) {
      RETVAL_TRUE;
    } else {
      RETVAL_FALSE;
    }

    break;
  case CASS_VALUE_TYPE_INET:
    // TODO: implement Inet
    RETVAL_NULL();
    break;
    // rc = cass_value_get_inet(value, &v_inet);
    // if (rc != CASS_OK) {
    //   php_error_docref(NULL TSRMLS_CC, E_WARNING,
    //     "Decoding error: %s", cass_error_desc(rc)
    //   );
    //   RETVAL_NULL();
    //   break;
    // }
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
    mpz_import(decimal_number->value, v_decimal.varint.size, 1, sizeof(v_decimal.varint.data[0]), 1, 0, v_decimal.varint.data);
    decimal_number->scale = v_decimal.scale;
    php_cassandra_format_integer(decimal_number->value, &string, &string_len);

    zend_update_property_stringl(cassandra_ce_Decimal, return_value, "value", strlen("value"), string, string_len TSRMLS_CC);
    zend_update_property_long(cassandra_ce_Decimal, return_value, "scale", strlen("scale"), (long) v_decimal.scale);
    efree(string);
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
    rc = cass_value_get_float(value, &v_float);

    if (rc != CASS_OK) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING,
        "Decoding error: %s", cass_error_desc(rc)
      );
      RETVAL_NULL();
      break;
    }

    RETVAL_DOUBLE(v_float);
    break;
  case CASS_VALUE_TYPE_LIST:
    // TODO: implement List
  case CASS_VALUE_TYPE_MAP:
    // TODO: implement Map
  case CASS_VALUE_TYPE_SET:
    // TODO: implement Set
    RETVAL_NULL();
    break;
  default:
    RETVAL_NULL();
  }

  return return_value;
}
