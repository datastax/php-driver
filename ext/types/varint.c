#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "math.h"
#include "varint.h"

extern zend_class_entry *cassandra_ce_InvalidArgumentException;

zend_class_entry *cassandra_ce_Varint = NULL;

/* {{{ Cassandra\Varint::__construct(string) */
PHP_METHOD(CassandraVarint, __construct)
{
  char *value;
  int value_len;
  cassandra_varint* number;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
    return;
  }

  number = (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (!php_cassandra_parse_integer(value, value_len, &number->value))
    return;

  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  zend_update_property_stringl(cassandra_ce_Varint, getThis(), "value", strlen("value"), string, string_len TSRMLS_CC);
  efree(string);
}
/* }}} */

/* {{{ Cassandra\Varint::__toString() */
PHP_METHOD(CassandraVarint, __toString)
{
  zval *zode = zend_read_property(cassandra_ce_Varint, getThis(), "value", strlen("value"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

/* {{{ Cassandra\Varint::value() */
PHP_METHOD(CassandraVarint, value)
{
  zval *zode = zend_read_property(cassandra_ce_Varint, getThis(), "value", strlen("value"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

static zend_function_entry CassandraVarint_methods[] = {
  PHP_ME(CassandraVarint, __construct, NULL, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraVarint, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraVarint, value, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static void
php_cassandra_varint_free(void *object TSRMLS_DC)
{
  cassandra_varint* number = (cassandra_varint*) object;

  mpz_clear(number->value);
  zend_object_std_dtor(&number->zval TSRMLS_CC);

  efree(number);
}

static zend_object_value
php_cassandra_varint_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_varint *number;

  number = (cassandra_varint*) emalloc(sizeof(cassandra_varint));
  memset(number, 0, sizeof(cassandra_varint));

  mpz_init(number->value);
  zend_object_std_init(&number->zval, class_type TSRMLS_CC);
  object_properties_init(&number->zval, class_type TSRMLS_CC);

  retval.handle = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_varint_free, NULL TSRMLS_CC);
  retval.handlers = zend_get_std_object_handlers();

  return retval;
}

void cassandra_define_CassandraVarint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Varint", CassandraVarint_methods);
  cassandra_ce_Varint = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_Varint->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Varint->create_object = php_cassandra_varint_new;

  /* fields */
  zend_declare_property_string(cassandra_ce_Varint, "value", strlen("value"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
}
