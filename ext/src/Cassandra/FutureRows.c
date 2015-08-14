#include "php_cassandra.h"
#include "util/future.h"
#include "util/result.h"
#include "util/ref.h"

zend_class_entry *cassandra_future_rows_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(FutureRows, get)
{
  zval* timeout = NULL;
  cassandra_rows* rows = NULL;
  const CassResult* result = NULL;

  cassandra_future_rows* self =
    (cassandra_future_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->rows) {
    RETURN_ZVAL(self->rows, 1, 0);
  }

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  if (php_cassandra_future_wait_timed(self->future, timeout TSRMLS_CC) == FAILURE) {
    return;
  }

  if (php_cassandra_future_is_error(self->future TSRMLS_CC) == FAILURE) {
    return;
  }

  result = cass_future_get_result(self->future);
  
  if (!result) {
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC,
                            "Future doesn't contain a result.");
    return;
  }

  MAKE_STD_ZVAL(self->rows);
  object_init_ex(self->rows, cassandra_rows_ce);
  rows = (cassandra_rows*) zend_object_store_get_object(self->rows TSRMLS_CC);

  if (php_cassandra_get_result(result, &rows->rows TSRMLS_CC) == FAILURE) {
    cass_result_free(result);
    zval_ptr_dtor(&self->rows);
    return;
  }

  if (cass_result_has_more_pages(result)) {
    Z_ADDREF_P(self->session);
    rows->statement = php_cassandra_add_ref(self->statement);
    rows->session   = self->session;
    rows->result    = result;
  } else {
    cass_result_free(result);
  }

  RETURN_ZVAL(self->rows, 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_future_rows_methods[] = {
  PHP_ME(FutureRows, get, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_future_rows_handlers;

static HashTable*
php_cassandra_future_rows_properties(zval *object TSRMLS_DC)
{
  /* cassandra_future_rows* self = (cassandra_future_rows*) zend_object_store_get_object(object TSRMLS_CC); */
  HashTable* props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_future_rows_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_future_rows_free(void *object TSRMLS_DC)
{
  cassandra_future_rows* future = (cassandra_future_rows*) object;

  zend_object_std_dtor(&future->zval TSRMLS_CC);

  if (future->rows) {
    zval_ptr_dtor(&future->rows);
    future->rows = NULL;
  }

  if (future->statement)
    php_cassandra_del_ref(&future->statement);

  if (future->session) {
    zval_ptr_dtor(&future->session);
    future->session = NULL;
  }

  if (future->future) {
    cass_future_free(future->future);
    future->future = NULL;
  }

  efree(future);
}

static zend_object_value
php_cassandra_future_rows_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_future_rows *future;

  future = (cassandra_future_rows*) ecalloc(1, sizeof(cassandra_future_rows));

  zend_object_std_init(&future->zval, class_type TSRMLS_CC);
  object_properties_init(&future->zval, class_type);

  future->future    = NULL;
  future->rows      = NULL;
  future->statement = NULL;
  future->session   = NULL;

  retval.handle   = zend_objects_store_put(future,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_future_rows_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_future_rows_handlers;

  return retval;
}

void cassandra_define_FutureRows(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\FutureRows", cassandra_future_rows_methods);
  cassandra_future_rows_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_future_rows_ce TSRMLS_CC, 1, cassandra_future_ce);
  cassandra_future_rows_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_future_rows_ce->create_object = php_cassandra_future_rows_new;

  memcpy(&cassandra_future_rows_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_future_rows_handlers.get_properties  = php_cassandra_future_rows_properties;
  cassandra_future_rows_handlers.compare_objects = php_cassandra_future_rows_compare;
}
