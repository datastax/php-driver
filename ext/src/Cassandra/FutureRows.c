#include "php_cassandra.h"
#include "util/future.h"
#include "util/result.h"
#include "util/ref.h"

zend_class_entry *cassandra_future_rows_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

static void
php_cassandra_future_clear(cassandra_future_rows *self)
{
  if (self->statement) {
    php_cassandra_del_ref(&self->statement);
    self->statement = NULL;
  }

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->session);
  if (self->future) {
    cass_future_free(self->future);
    self->future = NULL;
  }
}

PHP_METHOD(FutureRows, get)
{
  zval *timeout = NULL;
  cassandra_rows *rows = NULL;
  const CassResult *result = NULL;

  cassandra_future_rows *self = PHP_CASSANDRA_GET_FUTURE_ROWS(getThis());

  if (!PHP5TO7_ZVAL_IS_UNDEF(self->rows)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->rows), 1, 0);
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

  PHP5TO7_ZVAL_MAYBE_MAKE(self->rows);
  object_init_ex(PHP5TO7_ZVAL_MAYBE_P(self->rows), cassandra_rows_ce);
  rows = PHP_CASSANDRA_GET_ROWS(PHP5TO7_ZVAL_MAYBE_P(self->rows));

  if (php_cassandra_get_result(result, &rows->rows TSRMLS_CC) == FAILURE) {
    cass_result_free(result);
    zval_ptr_dtor(&self->rows);
    PHP5TO7_ZVAL_UNDEF(self->rows);
    return;
  }

  if (cass_result_has_more_pages(result)) {
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(rows->session),
                      PHP5TO7_ZVAL_MAYBE_P(self->session));
    rows->statement = php_cassandra_add_ref(self->statement);
    rows->result    = result;
  } else {
    cass_result_free(result);
  }

  php_cassandra_future_clear(self);

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->rows), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_future_rows_methods[] = {
  PHP_ME(FutureRows, get, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_future_rows_handlers;

static HashTable *
php_cassandra_future_rows_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

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
php_cassandra_future_rows_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_future_rows *self = PHP5TO7_ZEND_OBJECT_GET(future_rows, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->rows);

  php_cassandra_future_clear(self);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_future_rows_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_future_rows *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(future_rows, ce);

  self->future    = NULL;
  self->statement = NULL;
  PHP5TO7_ZVAL_UNDEF(self->rows);
  PHP5TO7_ZVAL_UNDEF(self->session);

  PHP5TO7_ZEND_OBJECT_INIT(future_rows, self, ce);
}

void cassandra_define_FutureRows(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\FutureRows", cassandra_future_rows_methods);
  cassandra_future_rows_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_future_rows_ce TSRMLS_CC, 1, cassandra_future_ce);
  cassandra_future_rows_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_future_rows_ce->create_object = php_cassandra_future_rows_new;

  memcpy(&cassandra_future_rows_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_future_rows_handlers.get_properties  = php_cassandra_future_rows_properties;
  cassandra_future_rows_handlers.compare_objects = php_cassandra_future_rows_compare;
}
