#include "php_cassandra.h"

zend_class_entry *cassandra_ssl_ce = NULL;

static zend_function_entry cassandra_ssl_methods[] = {
  PHP_FE_END
};

static zend_object_handlers cassandra_ssl_handlers;

static HashTable*
php_cassandra_ssl_properties(zval *object TSRMLS_DC)
{
  HashTable* props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_ssl_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_ssl_free(void *object TSRMLS_DC)
{
  cassandra_ssl* options = (cassandra_ssl*) object;

  zend_object_std_dtor(&options->zval TSRMLS_CC);
  cass_ssl_free(options->ssl);
  efree(options);
}

static zend_object_value
php_cassandra_ssl_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_ssl *options;

  options = (cassandra_ssl*) ecalloc(1, sizeof(cassandra_ssl));

  zend_object_std_init(&options->zval, class_type TSRMLS_CC);
  object_properties_init(&options->zval, class_type);

  options->ssl = cass_ssl_new();

  retval.handle   = zend_objects_store_put(options,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_ssl_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_ssl_handlers;

  return retval;
}

void cassandra_define_SSLOptions(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\SSLOptions", cassandra_ssl_methods);
  cassandra_ssl_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ssl_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_ssl_ce->create_object = php_cassandra_ssl_new;

  memcpy(&cassandra_ssl_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_ssl_handlers.get_properties  = php_cassandra_ssl_properties;
  cassandra_ssl_handlers.compare_objects = php_cassandra_ssl_compare;
}
