#include "php_cassandra.h"
#include "php_ini.h"
#include "ext/standard/info.h"

ZEND_DECLARE_MODULE_GLOBALS(cassandra)

const zend_function_entry cassandra_functions[] = {
  PHP_FE(cassandra_error_message, NULL)
  /* CassCluster */
  PHP_FE(cassandra_cluster_new, NULL)
  PHP_FE(cassandra_cluster_free, NULL)
  PHP_FE(cassandra_cluster_set_contact_points, NULL)
  /* CassSession */
  PHP_FE(cassandra_session_new, NULL)
  PHP_FE(cassandra_session_free, NULL)
  PHP_FE(cassandra_session_connect, NULL)
  /* CassFuture */
  PHP_FE(cassandra_future_free, NULL)
  PHP_FE(cassandra_future_error_code, NULL)
  PHP_FE(cassandra_future_error_message, NULL)
  PHP_FE_END /* Must be the last line in cassandra_functions[] */
};

zend_module_entry cassandra_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
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
  }
}

static int le_cassandra_session_res;
static void
php_cassandra_session_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassSession* session = (CassSession*) rsrc->ptr;

  if (session) {
    cass_session_free(session);
  }
}

static int le_cassandra_future_res;
static void
php_cassandra_future_dtor(zend_rsrc_list_entry* rsrc TSRMLS_DC)
{
  CassFuture* future = (CassFuture*) rsrc->ptr;

  if (future) {
    cass_future_free(future);
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

  RETURN_STRING(cass_error_desc(code), true);
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

PHP_FUNCTION(cassandra_future_free)
{
  CassFuture* future;
  zval* future_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &future_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_future_res);

  zend_list_delete(Z_RESVAL_P(future_resource));

  RETURN_TRUE;
}

PHP_FUNCTION(cassandra_future_error_code)
{
  CassFuture* future;
  zval* future_resource;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &future_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(future, CassFuture*, &future_resource, -1,
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_future_res);


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
    PHP_CASSANDRA_SESSION_RES_NAME, le_cassandra_future_res);

  message = cass_future_error_message(future);
  RETURN_STRINGL(message.data, message.length, true);
}
