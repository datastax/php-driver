#include "../php_cassandra.h"
#include "util/math.h"
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
}
/* }}} */

/* {{{ Cassandra\Varint::__toString() */
PHP_METHOD(CassandraVarint, __toString)
{
  cassandra_varint* number = (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

/* {{{ Cassandra\Varint::value() */
PHP_METHOD(CassandraVarint, value)
{
  cassandra_varint* number = (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry CassandraVarint_methods[] = {
  PHP_ME(CassandraVarint, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraVarint, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraVarint, value, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_varint_handlers;

static HashTable*
php_cassandra_varint_properties(zval *object TSRMLS_DC)
{
  cassandra_varint* number = (cassandra_varint*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*        props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_varint_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_varint* varint1 = (cassandra_varint*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_varint* varint2 = (cassandra_varint*) zend_object_store_get_object(obj2 TSRMLS_CC);

  return mpz_cmp(varint1->value, varint2->value);
}

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

  retval.handle   = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_varint_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_varint_handlers;

  return retval;
}

void cassandra_define_CassandraVarint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Varint", CassandraVarint_methods);
  cassandra_ce_Varint = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_varint_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_varint_handlers.get_properties = php_cassandra_varint_properties;
  cassandra_varint_handlers.compare_objects = php_cassandra_varint_compare;
  cassandra_ce_Varint->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Varint->create_object = php_cassandra_varint_new;
}
