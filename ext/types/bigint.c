#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "bigint.h"

extern zend_class_entry *cassandra_ce_InvalidArgumentException;

zend_class_entry *cassandra_ce_Bigint = NULL;

static int
ctype_digit(const char *s, int len)
{
  for (int i = 0; i < len; i++) {
    if (!isdigit(s[i]))
      return 0;
  }

  return 1;
}

/* {{{ Cassandra\Bigint::__construct(string) */
PHP_METHOD(CassandraBigint, __construct)
{
  char *value;
  int value_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
    return;
  }

  if (!ctype_digit(value, value_len)) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid integer value \"%s\"", value);
    return;
  }

  zend_update_property_stringl(cassandra_ce_Bigint, getThis(), "value", strlen("value"), value, value_len TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Bigint::__toString() */
PHP_METHOD(CassandraBigint, __toString)
{
  zval *zode = zend_read_property(cassandra_ce_Bigint, getThis(), "value", strlen("value"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), true);
}
/* }}} */

/* {{{ Cassandra\Bigint::value() */
PHP_METHOD(CassandraBigint, value)
{
  zval *zode = zend_read_property(cassandra_ce_Bigint, getThis(), "value", strlen("value"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), true);
}
/* }}} */

static zend_function_entry CassandraBigint_methods[] = {
  PHP_ME(CassandraBigint, __construct, NULL, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraBigint, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraBigint, value, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

void cassandra_define_CassandraBigint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Bigint", CassandraBigint_methods);
  cassandra_ce_Bigint = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_Bigint->ce_flags |= ZEND_ACC_FINAL_CLASS;

  /* fields */
  zend_declare_property_string(cassandra_ce_Bigint, "value", strlen("value"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
}
