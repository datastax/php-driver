#include "php_cassandra.h"
#include "util/future.h"

zend_class_entry *cassandra_future_close_ce = NULL;

PHP_METHOD(FutureClose, get)
{
  zval *timeout = NULL;
  cassandra_future_close *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_FUTURE_CLOSE(getThis());

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

static HashTable *
php_cassandra_future_close_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

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
php_cassandra_future_close_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_future_close *self =
      PHP5TO7_ZEND_OBJECT_GET(future_close, object);

  if (self->future)
    cass_future_free(self->future);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_future_close_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_future_close *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(future_close, ce);

  self->future = NULL;

  PHP5TO7_ZEND_OBJECT_INIT(future_close, self, ce);
}

void cassandra_define_FutureClose(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\FutureClose", cassandra_future_close_methods);
  cassandra_future_close_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_future_close_ce TSRMLS_CC, 1, cassandra_future_ce);
  cassandra_future_close_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_future_close_ce->create_object = php_cassandra_future_close_new;

  memcpy(&cassandra_future_close_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_future_close_handlers.get_properties  = php_cassandra_future_close_properties;
  cassandra_future_close_handlers.compare_objects = php_cassandra_future_close_compare;
}
