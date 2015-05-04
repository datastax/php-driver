#include "php_cassandra.h"
#include "util/bytes.h"

extern zend_class_entry *cassandra_invalid_argument_exception_ce;

zend_class_entry *cassandra_blob_ce = NULL;

/* {{{ Cassandra\Types\Blob::__construct(string) */
PHP_METHOD(Blob, __construct)
{
  char *bytes;
  int bytes_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &bytes, &bytes_len) == FAILURE) {
    return;
  }

  cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(getThis() TSRMLS_CC);
  blob->data = emalloc(bytes_len + 1 * sizeof(cass_byte_t));
  blob->size = bytes_len + 1;
  memcpy(blob->data, bytes, bytes_len + 1);
}
/* }}} */

/* {{{ Cassandra\Types\Blob::__toString() */
PHP_METHOD(Blob, __toString)
{
  cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(getThis() TSRMLS_CC);
  char* hex;
  int hex_len;
  php_cassandra_bytes_to_hex((const char *) blob->data, blob->size, &hex, &hex_len);

  RETURN_STRINGL(hex, hex_len, 0);
}
/* }}} */

/* {{{ Cassandra\Types\Blob::bytes() */
PHP_METHOD(Blob, bytes)
{
  cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(getThis() TSRMLS_CC);
  char* hex;
  int hex_len;
  php_cassandra_bytes_to_hex((const char *) blob->data, blob->size, &hex, &hex_len);

  RETURN_STRINGL(hex, hex_len, 0);
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
  PHP_FE_END
};

static zend_object_handlers cassandra_blob_handlers;

static HashTable*
php_cassandra_blob_properties(zval *object TSRMLS_DC)
{
  cassandra_blob* blob  = (cassandra_blob*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*      props = zend_std_get_properties(object TSRMLS_CC);

  zval* bytes;
  char* hex;
  int hex_len;
  php_cassandra_bytes_to_hex((const char *) blob->data, blob->size, &hex, &hex_len);

  MAKE_STD_ZVAL(bytes);
  ZVAL_STRINGL(bytes, hex, hex_len, 0);

  zend_hash_update(props, "bytes", sizeof("bytes"), &bytes, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_blob_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_blob* blob1 = NULL;
  cassandra_blob* blob2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  blob1 = (cassandra_blob*) zend_object_store_get_object(obj1 TSRMLS_CC);
  blob2 = (cassandra_blob*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (blob1->size == blob2->size) {
    return memcmp((const char*) blob1->data, (const char*) blob2->data, blob1->size);
  } else if (blob1->size < blob2->size) {
    return -1;
  } else {
    return 1;
  }
}

static void
php_cassandra_blob_free(void *object TSRMLS_DC)
{
  cassandra_blob* blob = (cassandra_blob*) object;

  zend_object_std_dtor(&blob->zval TSRMLS_CC);

  efree(blob->data);
  efree(blob);
}

static zend_object_value
php_cassandra_blob_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_blob *blob;

  blob = (cassandra_blob*) emalloc(sizeof(cassandra_blob));
  memset(blob, 0, sizeof(cassandra_blob));

  zend_object_std_init(&blob->zval, class_type TSRMLS_CC);
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&blob->zval, class_type);
#else
  zend_hash_copy(blob->zval.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void*) NULL, sizeof(zval*));
#endif

  retval.handle   = zend_objects_store_put(blob, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_blob_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_blob_handlers;

  return retval;
}

void cassandra_define_Blob(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Types\\Blob", cassandra_blob_methods);
  cassandra_blob_ce = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_blob_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_blob_handlers.get_properties = php_cassandra_blob_properties;
  cassandra_blob_handlers.compare_objects = php_cassandra_blob_compare;
  cassandra_blob_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_blob_ce->create_object = php_cassandra_blob_new;
}
