#include "php_cassandra.h"
#include "util/bytes.h"

zend_class_entry *cassandra_blob_ce = NULL;

void
php_cassandra_blob_init(INTERNAL_FUNCTION_PARAMETERS)
{
  cassandra_blob *self;
  char *string;
  php5to7_size string_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_blob_ce TSRMLS_CC)) {
    self = PHP_CASSANDRA_GET_BLOB(getThis());
  } else {
    object_init_ex(return_value, cassandra_blob_ce);
    self = PHP_CASSANDRA_GET_BLOB(return_value);
  }

  self->data = emalloc(string_len * sizeof(cass_byte_t));
  self->size = string_len;
  memcpy(self->data, string, string_len);
}

/* {{{ Cassandra\Blob::__construct(string) */
PHP_METHOD(Blob, __construct)
{
  php_cassandra_blob_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Blob::__toString() */
PHP_METHOD(Blob, __toString)
{
  cassandra_blob *blob = PHP_CASSANDRA_GET_BLOB(getThis());
  char *hex;
  int hex_len;
  php_cassandra_bytes_to_hex((const char *) blob->data, blob->size, &hex, &hex_len);

  PHP5TO7_RETVAL_STRINGL(hex, hex_len);
  efree(hex);
}
/* }}} */

/* {{{ Cassandra\Blob::bytes() */
PHP_METHOD(Blob, bytes)
{
  cassandra_blob *blob = PHP_CASSANDRA_GET_BLOB(getThis());
  char *hex;
  int hex_len;
  php_cassandra_bytes_to_hex((const char *) blob->data, blob->size, &hex, &hex_len);

  PHP5TO7_RETVAL_STRINGL(hex, hex_len);
  efree(hex);
}
/* }}} */

/* {{{ Cassandra\Blob::toBinaryString() */
PHP_METHOD(Blob, toBinaryString)
{
  cassandra_blob *blob = PHP_CASSANDRA_GET_BLOB(getThis());

  PHP5TO7_RETVAL_STRINGL((const char *)blob->data, blob->size);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, bytes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_blob_methods[] = {
  PHP_ME(Blob, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Blob, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Blob, bytes, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Blob, toBinaryString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_blob_handlers;

static HashTable*
php_cassandra_blob_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_blob_properties(zval *object TSRMLS_DC)
{
  cassandra_blob *self = PHP_CASSANDRA_GET_BLOB(object);
  HashTable      *props = zend_std_get_properties(object TSRMLS_CC);
  php5to7_zval    bytes;

  char *hex;
  int hex_len;
  php_cassandra_bytes_to_hex((const char *) self->data, self->size, &hex, &hex_len);

  PHP5TO7_ZVAL_MAYBE_MAKE(bytes);
  PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(bytes), hex, hex_len);
  efree(hex);

  PHP5TO7_ZEND_HASH_UPDATE(props, "bytes", sizeof("bytes"), PHP5TO7_ZVAL_MAYBE_P(bytes), sizeof(zval));

  return props;
}

static int
php_cassandra_blob_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_blob *blob1 = NULL;
  cassandra_blob *blob2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  blob1 = PHP_CASSANDRA_GET_BLOB(obj1);
  blob2 = PHP_CASSANDRA_GET_BLOB(obj2);

  if (blob1->size == blob2->size) {
    return memcmp((const char*) blob1->data, (const char*) blob2->data, blob1->size);
  } else if (blob1->size < blob2->size) {
    return -1;
  } else {
    return 1;
  }
}

static void
php_cassandra_blob_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_blob *self = PHP5TO7_ZEND_OBJECT_GET(blob, object);

  efree(self->data);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_blob_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_blob *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(blob, ce);

  PHP5TO7_ZEND_OBJECT_INIT(blob, self, ce);
}

void cassandra_define_Blob(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Blob", cassandra_blob_methods);
  cassandra_blob_ce = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_blob_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_blob_handlers.get_properties  = php_cassandra_blob_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_blob_handlers.get_gc          = php_cassandra_blob_gc;
#endif
  cassandra_blob_handlers.compare_objects = php_cassandra_blob_compare;
  cassandra_blob_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_blob_ce->create_object = php_cassandra_blob_new;
}
