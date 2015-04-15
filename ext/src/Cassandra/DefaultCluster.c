#include "php_cassandra.h"

zend_class_entry *cassandra_default_cluster_ce = NULL;

PHP_METHOD(DefaultCluster, connect)
{

}

PHP_METHOD(DefaultCluster, connectAsync)
{

}

ZEND_BEGIN_ARG_INFO_EX(arginfo_keyspace, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_default_cluster_methods[] = {
  PHP_ME(DefaultCluster, connect, arginfo_keyspace, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultCluster, connectAsync, arginfo_keyspace,
    ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_default_cluster_handlers;

static HashTable*
php_cassandra_default_cluster_properties(zval *object TSRMLS_DC)
{
  cassandra_cluster* cluster = (cassandra_cluster*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*         props   = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_default_cluster_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_default_cluster_free(void *object TSRMLS_DC)
{
  cassandra_cluster* cluster = (cassandra_cluster*) object;

  zend_object_std_dtor(&cluster->zval TSRMLS_CC);

  if (!cluster->persist)
    cass_cluster_free(cluster->cluster);

  efree(cluster);
}

static zend_object_value
php_cassandra_default_cluster_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_cluster *cluster;

  cluster = (cassandra_cluster*) ecalloc(1, sizeof(cassandra_cluster));

  zend_object_std_init(&cluster->zval, class_type TSRMLS_CC);
  object_properties_init(&cluster->zval, class_type);

  cluster->default_consistency     = CASS_CONSISTENCY_ONE;
  cluster->default_page_size       = 10000;
  cluster->default_timeout         = NULL;
  cluster->persist                 = 0;

  retval.handle   = zend_objects_store_put(cluster,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_default_cluster_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_default_cluster_handlers;

  return retval;
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(DefaultCluster)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Cluster", cassandra_default_cluster_methods);
  cassandra_default_cluster_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_default_cluster_ce TSRMLS_CC, 1, cassandra_cluster_ce);
  cassandra_default_cluster_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_default_cluster_ce->create_object = php_cassandra_default_cluster_new;

  memcpy(&cassandra_default_cluster_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_default_cluster_handlers.get_properties  = php_cassandra_default_cluster_properties;
  cassandra_default_cluster_handlers.compare_objects = php_cassandra_default_cluster_compare;

  return SUCCESS;
}
