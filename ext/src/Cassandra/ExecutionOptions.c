#include "php_cassandra.h"

#include "util/consistency.h"

zend_class_entry *cassandra_execution_options_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(ExecutionOptions, __construct)
{
  zval* options = NULL;
  cassandra_execution_options* self = NULL;
  zval** consistency = NULL;
  zval** serial_consistency = NULL;
  zval** page_size = NULL;
  zval** timeout = NULL;
  zval** arguments = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &options) == FAILURE) {
    return;
  }

  if (!options) return;

  if (Z_TYPE_P(options) != IS_ARRAY) {
    INVALID_ARGUMENT(options, "an array");
  }

  self = (cassandra_execution_options*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_find(Z_ARRVAL_P(options), "consistency", sizeof("consistency"), (void**)&consistency) == SUCCESS) {
    if (php_cassandra_get_consistency(*consistency, &self->consistency TSRMLS_CC) == FAILURE) {
      return;
    }
  }

  if (zend_hash_find(Z_ARRVAL_P(options), "serial_consistency", sizeof("serial_consistency"), (void**)&serial_consistency) == SUCCESS) {
    if (php_cassandra_get_serial_consistency(*serial_consistency, &self->serial_consistency TSRMLS_CC) == FAILURE) {
      return;
    }
  }

  if (zend_hash_find(Z_ARRVAL_P(options), "page_size", sizeof("page_size"), (void**)&page_size) == SUCCESS) {
    if (Z_TYPE_P(*page_size) != IS_LONG || Z_LVAL_P(*page_size) <= 0) {
      INVALID_ARGUMENT(*page_size, "greater than zero");
    }
    self->page_size = Z_LVAL_P(*page_size);
  }

  if (zend_hash_find(Z_ARRVAL_P(options), "timeout", sizeof("timeout"), (void**)&timeout) == SUCCESS) {
    if (!(Z_TYPE_P(*timeout) == IS_LONG   && Z_LVAL_P(*timeout) > 0) &&
        !(Z_TYPE_P(*timeout) == IS_DOUBLE && Z_DVAL_P(*timeout) > 0) &&
        !(Z_TYPE_P(*timeout) == IS_NULL)) {
      INVALID_ARGUMENT(*timeout, "a number of seconds greater than zero or null");
    }

    self->timeout = *timeout;
    Z_ADDREF_P(self->timeout);
  }

  if (zend_hash_find(Z_ARRVAL_P(options), "arguments", sizeof("arguments"), (void**)&arguments) == SUCCESS) {
    if (Z_TYPE_P(*arguments) != IS_ARRAY) {
      INVALID_ARGUMENT(*arguments, "an array");
      return;
    }
    self->arguments = *arguments;
    Z_ADDREF_P(self->arguments);
  }
}

PHP_METHOD(ExecutionOptions, __get)
{
  char *name;
  int   name_len;

  cassandra_execution_options* self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = (cassandra_execution_options*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (name_len == 11 && strncmp("consistency", name, name_len) == 0) {
    RETURN_LONG(self->consistency);
  } else if (name_len == 17 && strncmp("serialConsistency", name, name_len) == 0) {
    RETURN_LONG(self->serial_consistency);
  } else if (name_len == 8 && strncmp("pageSize", name, name_len) == 0) {
    RETURN_LONG(self->page_size);
  } else if (name_len == 7 && strncmp("timeout", name, name_len) == 0) {
    if (self->timeout) {
      RETURN_ZVAL(self->timeout, 1, 0);
    } else {
      RETURN_NULL();
    }
  } else if (name_len == 9 && strncmp("arguments", name, name_len) == 0) {
    if (self->arguments) {
      RETURN_ZVAL(self->arguments, 1, 0);
    } else {
      RETURN_NULL();
    }
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

static HashTable*
php_cassandra_execution_options_properties(zval *object TSRMLS_DC)
{
  HashTable* props = zend_std_get_properties(object TSRMLS_CC);

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
php_cassandra_execution_options_free(void *object TSRMLS_DC)
{
  cassandra_execution_options* options = (cassandra_execution_options*) object;

  if (options->arguments) {
    zval_ptr_dtor(&options->arguments);
    options->arguments = NULL;
  }

  zend_object_std_dtor(&options->zval TSRMLS_CC);
  efree(options);
}

static zend_object_value
php_cassandra_execution_options_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_execution_options *options;

  options = (cassandra_execution_options*) ecalloc(1, sizeof(cassandra_execution_options));

  zend_object_std_init(&options->zval, class_type TSRMLS_CC);
  object_properties_init(&options->zval, class_type);

  options->consistency = -1;
  options->serial_consistency = -1;
  options->page_size = -1;
  options->timeout = NULL;
  options->arguments = NULL;

  retval.handle   = zend_objects_store_put(options,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_execution_options_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_execution_options_handlers;

  return retval;
}

void cassandra_define_ExecutionOptions(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\ExecutionOptions", cassandra_execution_options_methods);
  cassandra_execution_options_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_execution_options_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_execution_options_ce->create_object = php_cassandra_execution_options_new;

  memcpy(&cassandra_execution_options_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_execution_options_handlers.get_properties  = php_cassandra_execution_options_properties;
  cassandra_execution_options_handlers.compare_objects = php_cassandra_execution_options_compare;
}
