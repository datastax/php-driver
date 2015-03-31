#include "../php_cassandra.h"
#include "util/math.h"
#include "decimal.h"

extern zend_class_entry *cassandra_ce_InvalidArgumentException;

zend_class_entry *cassandra_ce_Decimal = NULL;

/* {{{ Cassandra\Decimal::__construct(string) */
PHP_METHOD(CassandraDecimal, __construct)
{
  char *value;
  int value_len;
  int scale = 0;
  cassandra_decimal* number;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
    return;
  }

  number = (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (!php_cassandra_parse_decimal(value, value_len, &number->value, &scale))
    return;

  number->scale = scale;
}
/* }}} */

/* {{{ Cassandra\Decimal::__toString() */
PHP_METHOD(CassandraDecimal, __toString)
{
  cassandra_decimal* number = (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_decimal(number->value, number->scale, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

/* {{{ Cassandra\Decimal::value() */
PHP_METHOD(CassandraDecimal, value)
{
  cassandra_decimal* number = (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

PHP_METHOD(CassandraDecimal, scale)
{
  cassandra_decimal* number = (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_LONG(number->scale);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry CassandraDecimal_methods[] = {
  PHP_ME(CassandraDecimal, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraDecimal, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraDecimal, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraDecimal, scale, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_decimal_handlers;

static HashTable*
php_cassandra_decimal_properties(zval *object TSRMLS_DC)
{
  cassandra_decimal* number = (cassandra_decimal*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*         props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  zval* scale;
  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);
  MAKE_STD_ZVAL(scale);
  ZVAL_LONG(scale, number->scale);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);
  zend_hash_update(props, "scale", sizeof("scale"), &scale, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_decimal_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_decimal* decimal1 = (cassandra_decimal*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_decimal* decimal2 = (cassandra_decimal*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (decimal1->scale == decimal2->scale) {
    return mpz_cmp(decimal1->value, decimal2->value);
  } else if (decimal1->scale < decimal2->scale) {
    return -1;
  } else {
    return 1;
  }
}

static void
php_cassandra_decimal_free(void *object TSRMLS_DC)
{
  cassandra_decimal* number = (cassandra_decimal*) object;

  mpz_clear(number->value);
  zend_object_std_dtor(&number->zval TSRMLS_CC);

  efree(number);
}

static zend_object_value
php_cassandra_decimal_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_decimal *number;

  number = (cassandra_decimal*) emalloc(sizeof(cassandra_decimal));
  memset(number, 0, sizeof(cassandra_decimal));

  mpz_init(number->value);
  zend_object_std_init(&number->zval, class_type TSRMLS_CC);
  object_properties_init(&number->zval, class_type TSRMLS_CC);
  number->scale = 0;

  retval.handle   = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_decimal_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_decimal_handlers;

  return retval;
}

void cassandra_define_CassandraDecimal(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Decimal", CassandraDecimal_methods);
  cassandra_ce_Decimal = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_decimal_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_decimal_handlers.get_properties = php_cassandra_decimal_properties;
  cassandra_decimal_handlers.compare_objects = php_cassandra_decimal_compare;
  cassandra_ce_Decimal->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Decimal->create_object = php_cassandra_decimal_new;
}
