#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "blob.h"

extern zend_class_entry *cassandra_ce_InvalidArgumentException;

zend_class_entry *cassandra_ce_Blob = NULL;

static int
ctype_digit(const char *s, int len)
{
  int i;
  for (i = 0; i < len; i++) {
    if (!isdigit(s[i]))
      return 0;
    }

    return 1;
  }

  /* {{{ Cassandra\Blob::__construct(string) */
  PHP_METHOD(CassandraBlob, __construct)
{
  char *bytes;
  int bytes_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &bytes, &bytes) == FAILURE) {
    return;
  }

  zend_update_property_stringl(cassandra_ce_Blob, getThis(), "bytes", strlen("bytes"), bytes, bytes_len TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Blob::__toString() */
PHP_METHOD(CassandraBlob, __toString)
{
  zval *zode = zend_read_property(cassandra_ce_Blob, getThis(), "bytes", strlen("bytes"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

/* {{{ Cassandra\Blob::bytes() */
PHP_METHOD(CassandraBlob, bytes)
{
  zval *zode = zend_read_property(cassandra_ce_Blob, getThis(), "bytes", strlen("bytes"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

static zend_function_entry CassandraBlob_methods[] = {
  PHP_ME(CassandraBlob, __construct, NULL, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraBlob, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraBlob, bytes, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

void cassandra_define_CassandraBlob(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Blob", CassandraBlob_methods);
  cassandra_ce_Blob = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_Blob->ce_flags |= ZEND_ACC_FINAL_CLASS;

  /* fields */
  zend_declare_property_string(cassandra_ce_Blob, "bytes", strlen("bytes"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
}
