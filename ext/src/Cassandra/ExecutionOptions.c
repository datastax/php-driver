#include "php_cassandra.h"

#include "util/consistency.h"

zend_class_entry *cassandra_execution_options_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(ExecutionOptions, __construct)
{
  zval *options = NULL;
  cassandra_execution_options *self = NULL;
  zval *consistency = NULL;
  zval *serial_consistency = NULL;
  zval *page_size = NULL;
  zval *timeout = NULL;
  zval *arguments = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &options) == FAILURE) {
    return;
  }

  if (!options) return;

  if (Z_TYPE_P(options) != IS_ARRAY) {
    INVALID_ARGUMENT(options, "an array");
  }

  self = PHP_CASSANDRA_GET_EXECUTION_OPTIONS(getThis());

  if (PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "consistency", sizeof("consistency"), consistency)) {
    if (php_cassandra_get_consistency(consistency, &self->consistency TSRMLS_CC) == FAILURE) {
      return;
    }
  }

  if (PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "serial_consistency", sizeof("serial_consistency"), serial_consistency)) {
    if (php_cassandra_get_serial_consistency(serial_consistency, &self->serial_consistency TSRMLS_CC) == FAILURE) {
      return;
    }
  }

  if (PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "page_size", sizeof("page_size"), page_size)) {
    if (Z_TYPE_P(page_size) != IS_LONG || Z_LVAL_P(page_size) <= 0) {
      INVALID_ARGUMENT(page_size, "greater than zero");
    }
    self->page_size = Z_LVAL_P(page_size);
  }

  if (PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "timeout", sizeof("timeout"), timeout)) {
    if (!(Z_TYPE_P(timeout) == IS_LONG   && Z_LVAL_P(timeout) > 0) &&
        !(Z_TYPE_P(timeout) == IS_DOUBLE && Z_DVAL_P(timeout) > 0) &&
        !(Z_TYPE_P(timeout) == IS_NULL)) {
      INVALID_ARGUMENT(timeout, "a number of seconds greater than zero or null");
    }

    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(self->timeout), timeout);
  }

  if (PHP5TO7_ZEND_HASH_FIND(Z_ARRVAL_P(options), "arguments", sizeof("arguments"), arguments)) {
    if (Z_TYPE_P(arguments) != IS_ARRAY) {
      INVALID_ARGUMENT(arguments, "an array");
      return;
    }
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(self->arguments), arguments);
  }
}

PHP_METHOD(ExecutionOptions, __get)
{
  char *name;
  int   name_len;

  cassandra_execution_options *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_EXECUTION_OPTIONS(getThis());

  if (name_len == 11 && strncmp("consistency", name, name_len) == 0) {
    if (self->consistency == -1) {
      RETURN_NULL();
    }
    RETURN_LONG(self->consistency);
  } else if (name_len == 17 && strncmp("serialConsistency", name, name_len) == 0) {
    if (self->serial_consistency == -1) {
      RETURN_NULL();
    }
    RETURN_LONG(self->serial_consistency);
  } else if (name_len == 8 && strncmp("pageSize", name, name_len) == 0) {
    if (self->page_size == -1) {
      RETURN_NULL();
    }
    RETURN_LONG(self->page_size);
  } else if (name_len == 7 && strncmp("timeout", name, name_len) == 0) {
    if (PHP5TO7_ZVAL_IS_UNDEF(self->timeout)) {
      RETURN_NULL();
    }
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->timeout), 1, 0);
  } else if (name_len == 9 && strncmp("arguments", name, name_len) == 0) {
    if (PHP5TO7_ZVAL_IS_UNDEF(self->arguments)) {
      RETURN_NULL();
    }
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->arguments), 1, 0);
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo___get, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_execution_options_methods[] = {
  PHP_ME(ExecutionOptions, __construct, arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(ExecutionOptions, __get, arginfo___get, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_execution_options_handlers;

static HashTable *
php_cassandra_execution_options_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_execution_options_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_execution_options_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_execution_options *self = (cassandra_execution_options *) object;

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->arguments);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_ZEND_OBJECT_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_execution_options_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_execution_options *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(execution_options, ce);

  self->consistency = -1;
  self->serial_consistency = -1;
  self->page_size = -1;
  PHP5TO7_ZVAL_UNDEF(self->arguments);
  PHP5TO7_ZVAL_UNDEF(self->timeout);

  PHP5TO7_ZEND_OBJECT_INIT(execution_options, self, ce);

}

void cassandra_define_ExecutionOptions(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\ExecutionOptions", cassandra_execution_options_methods);
  cassandra_execution_options_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_execution_options_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_execution_options_ce->create_object = php_cassandra_execution_options_new;

  memcpy(&cassandra_execution_options_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_execution_options_handlers.get_properties  = php_cassandra_execution_options_properties;
  cassandra_execution_options_handlers.compare_objects = php_cassandra_execution_options_compare;
}
