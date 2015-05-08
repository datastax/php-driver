#include "php_cassandra.h"
#include "util/future.h"

zend_class_entry *cassandra_future_close_ce = NULL;

PHP_METHOD(FutureClose, get)
{
  zval* timeout = NULL;
  cassandra_future_close* self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE)
    return;

  self = (cassandra_future_close*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_future_wait_timed(self->future, timeout TSRMLS_CC) == FAILURE)
    return;

  if (php_cassandra_future_is_error(self->future TSRMLS_CC) == FAILURE)
    return;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_future_close_methods[] = {
  PHP_ME(FutureClose, get, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_future_close_handlers;

static HashTable*
php_cassandra_future_close_properties(zval *object TSRMLS_DC)
{
  /* cassandra_future_close* self = (cassandra_future_close*) zend_object_store_get_object(object TSRMLS_CC); */
  HashTable* props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_future_close_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_future_close_free(void *object TSRMLS_DC)
{
  cassandra_future_close* future = (cassandra_future_close*) object;

  zend_object_std_dtor(&future->zval TSRMLS_CC);

  if (future->future)
    cass_future_free(future->future);

  efree(future);
}

static zend_object_value
php_cassandra_future_close_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_future_close *future;

  future = (cassandra_future_close*) ecalloc(1, sizeof(cassandra_future_close));

  zend_object_std_init(&future->zval, class_type TSRMLS_CC);
  object_properties_init(&future->zval, class_type);

  future->future = NULL;

  retval.handle   = zend_objects_store_put(future,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_future_close_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_future_close_handlers;

  return retval;
}

void cassandra_define_FutureClose(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\FutureClose", cassandra_future_close_methods);
  cassandra_future_close_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_future_close_ce TSRMLS_CC, 1, cassandra_future_ce);
  cassandra_future_close_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_future_close_ce->create_object = php_cassandra_future_close_new;

  memcpy(&cassandra_future_close_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_future_close_handlers.get_properties  = php_cassandra_future_close_properties;
  cassandra_future_close_handlers.compare_objects = php_cassandra_future_close_compare;
}
