#include "../php_cassandra.h"
#include "bigint.h"

extern zend_class_entry* cassandra_ce_InvalidArgumentException;

zend_class_entry* cassandra_ce_Float = NULL;

static int
ctype_float(const char* s, int len)
{
  int i;
  int dot = -1;
  for (i = 0; i < len; i++) {
    if (s[i] == '.' && dot == -1)
      dot = i;

    if (!(isdigit(s[i]) || dot == i))
      return 0;
  }

  return 1;
}

/* {{{ Cassandra\Float::__construct(string) */
PHP_METHOD(CassandraFloat, __construct)
{
  char* value;
  int value_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
    return;
  }

  if (!ctype_float(value, value_len)) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid float value \"%s\"", value);
    return;
  }

  cassandra_float* number = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  number->value = (cass_float_t) strtof(value, NULL);
}
/* }}} */

/* {{{ Cassandra\Float::__toString() */
PHP_METHOD(CassandraFloat, __toString)
{
  char* string;
  cassandra_float* number = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  spprintf(&string, 0, "%.*F", (int) EG(precision), number->value);
  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Float::value() */
PHP_METHOD(CassandraFloat, value)
{
  cassandra_float* number = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG((long) number->value);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry CassandraFloat_methods[] = {
  PHP_ME(CassandraFloat, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraFloat, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraFloat, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_float_handlers;

static HashTable*
php_cassandra_float_properties(zval *object TSRMLS_DC)
{
  cassandra_float* number = (cassandra_float*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*       props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  int string_len;

  string_len = spprintf(&string, 0, "%.*F", (int) EG(precision), number->value);

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_float_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_float* number1 = (cassandra_float*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_float* number2 = (cassandra_float*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (number1->value == number2->value)
    return 0;
  else if (number1->value < number2->value)
    return -1;
  else
    return 1;
}

static void
php_cassandra_float_free(void *object TSRMLS_DC)
{
  cassandra_float* number = (cassandra_float*) object;

  zend_object_std_dtor(&number->zval TSRMLS_CC);

  efree(number);
}

static zend_object_value
php_cassandra_float_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_float *number;

  number = (cassandra_float*) emalloc(sizeof(cassandra_float));
  memset(number, 0, sizeof(cassandra_float));

  zend_object_std_init(&number->zval, class_type TSRMLS_CC);
  object_properties_init(&number->zval, class_type TSRMLS_CC);

  retval.handle   = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_float_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_float_handlers;

  return retval;
}

void cassandra_define_CassandraFloat(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Float", CassandraFloat_methods);
  cassandra_ce_Float = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_float_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_float_handlers.get_properties = php_cassandra_float_properties;
  cassandra_float_handlers.compare_objects = php_cassandra_float_compare;
  cassandra_ce_Float->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Float->create_object = php_cassandra_float_new;
}
