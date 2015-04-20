#include "php_cassandra.h"

zend_class_entry *cassandra_execute_options_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(ExecuteOptions, __construct)
{
  zval* options = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &options) == FAILURE) {
    return;
  }

  if (!options) {
    return;
  }

  if (Z_TYPE_P(options) != IS_ARRAY) {
    INVALID_ARGUMENT(options, "an array");
  }

  cassandra_execute_options* self =
    (cassandra_execute_options*) zend_object_store_get_object(getThis() TSRMLS_CC);

  zval* consistency;
  if (zend_hash_find(Z_ARRVAL_P(options), "consistency", sizeof("consistency"), (void**)&consistency) != FAILURE) {
    /* TODO: Check values */
    if (Z_TYPE_P(consistency) != IS_LONG) {
      INVALID_ARGUMENT(consistency, "a long");
    }
    self->consistency = Z_LVAL_P(consistency);
  }

  zval* serial_consistency;
  if (zend_hash_find(Z_ARRVAL_P(options), "serial_consistency", sizeof("serial_consistency"), (void**)&serial_consistency) != FAILURE) {
    /* TODO: Check values */
    if (Z_TYPE_P(serial_consistency) != IS_LONG) {
      INVALID_ARGUMENT(serial_consistency, "a long");
    }
    self->serial_consistency = Z_LVAL_P(serial_consistency);
  }

  zval* page_size;
  if (zend_hash_find(Z_ARRVAL_P(options), "page_size", sizeof("page_size"), (void**)&page_size) != FAILURE) {
    if (Z_TYPE_P(page_size) != IS_LONG) {
      INVALID_ARGUMENT(page_size, "a long");
    }
    self->page_size = Z_LVAL_P(page_size);
  }

  zval* timeout;
  if (zend_hash_find(Z_ARRVAL_P(options), "timeout", sizeof("timeout"), (void**)&timeout) != FAILURE) {
    if (Z_TYPE_P(timeout) != IS_LONG) {
      INVALID_ARGUMENT(timeout, "a long");
    }
    self->timeout = Z_LVAL_P(timeout);
  }

  zval* arguments;
  if (zend_hash_find(Z_ARRVAL_P(arguments), "arguments", sizeof("arguments"), (void**)&arguments) != FAILURE) {
    if (Z_TYPE_P(arguments) != IS_ARRAY) {
      INVALID_ARGUMENT(arguments, "an array");
    }
    Z_ADDREF_P(arguments);
    self->arguments = arguments;
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_ARRAY_INFO(0, options)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_execute_options_methods[] = {
  PHP_ME(ExecuteOptions, __construct, arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_FE_END
};

static zend_object_handlers cassandra_execute_options_handlers;

static HashTable*
php_cassandra_execute_options_properties(zval *object TSRMLS_DC)
{
  cassandra_execute_options*  options = (cassandra_execute_options*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*                  props     = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_execute_options_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_execute_options_free(void *object TSRMLS_DC)
{
  cassandra_execute_options* options = (cassandra_execute_options*) object;

  if (options->arguments) {
    Z_DELREF_P(options->arguments);
  }

  zend_object_std_dtor(&options->zval TSRMLS_CC);
  efree(options);
}

static zend_object_value
php_cassandra_execute_options_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_execute_options *options;

  options = (cassandra_execute_options*) ecalloc(1, sizeof(cassandra_execute_options));

  zend_object_std_init(&options->zval, class_type TSRMLS_CC);
  object_properties_init(&options->zval, class_type);

  options->consistency = -1;
  options->serial_consistency = -1;
  options->page_size = -1;
  options->timeout = -1;
  options->arguments = NULL;

  retval.handle   = zend_objects_store_put(options,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_execute_options_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_execute_options_handlers;

  return retval;
}

void cassandra_define_ExecuteOptions(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\ExecuteOptions", cassandra_execute_options_methods);
  cassandra_execute_options_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_execute_options_ce TSRMLS_CC, 1, cassandra_execute_options_ce);
  cassandra_execute_options_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_execute_options_ce->create_object = php_cassandra_execute_options_new;

  memcpy(&cassandra_execute_options_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_execute_options_handlers.get_properties  = php_cassandra_execute_options_properties;
  cassandra_execute_options_handlers.compare_objects = php_cassandra_execute_options_compare;
}
