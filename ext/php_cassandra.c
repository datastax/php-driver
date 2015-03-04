#include "php_cassandra.h"
#include "php_ini.h"
#include "ext/standard/info.h"

ZEND_DECLARE_MODULE_GLOBALS(cassandra)

const zend_function_entry cassandra_functions[] = {
  PHP_FE(cassandra_cluster_new, NULL)
  PHP_FE(cassandra_cluster_free, NULL)
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

PHP_MINIT_FUNCTION(cassandra)
{
  // REGISTER_INI_ENTRIES();
  le_cassandra_cluster_res = zend_register_list_destructors_ex(
    php_cassandra_cluster_dtor,
    NULL,
    PHP_CASSANDRA_CLUSTER_RES_NAME,
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
  zval *cluster_resource;
  char *data;
  int data_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &cluster_resource) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(cluster, CassCluster*, &cluster_resource, -1,
    PHP_CASSANDRA_CLUSTER_RES_NAME, le_cassandra_cluster_res);

  zend_list_delete(Z_RESVAL_P(cluster_resource));

  RETURN_TRUE;
}
