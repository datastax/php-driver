#include "php_cassandra.h"
#include <ext/standard/php_filestat.h>

zend_class_entry *cassandra_ssl_builder_ce = NULL;

static int
file_get_contents(char* path, char** output, int* len TSRMLS_DC)
{
  php_stream* stream = php_stream_open_wrapper(path, "rb",
                         USE_PATH|REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);

  if (!stream) {
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC,
      "The path '%s' doesn't exist or is not readable", path);
    return 0;
  }

  *len = php_stream_copy_to_mem(stream, output, PHP_STREAM_COPY_ALL, 0);

  php_stream_close(stream);
  return 1;
}

PHP_METHOD(SSLOptionsBuilder, build)
{
  cassandra_ssl* ssl = NULL;
  int   len;
  char* contents;
  CassError rc;

  cassandra_ssl_builder* builder =
    (cassandra_ssl_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_ssl_ce);
  ssl = (cassandra_ssl*) zend_object_store_get_object(return_value TSRMLS_CC);

  cass_ssl_set_verify_flags(ssl->ssl, builder->flags);

  if (builder->trusted_certs) {
    int   i;
    char* path;

    for (i = 0; i < builder->trusted_certs_cnt; i++) {
      path = builder->trusted_certs[i];

      if (!file_get_contents(path, &contents, &len TSRMLS_CC))
        return;

      rc = cass_ssl_add_trusted_cert_n(ssl->ssl, contents, len);
      efree(contents);
      ASSERT_SUCCESS(rc);
    }
  }

  if (builder->client_cert) {
    if (!file_get_contents(builder->client_cert, &contents, &len TSRMLS_CC))
      return;

    rc = cass_ssl_set_cert_n(ssl->ssl, contents, len);
    efree(contents);
    ASSERT_SUCCESS(rc);
  }

  if (builder->private_key) {
    if (!file_get_contents(builder->private_key, &contents, &len TSRMLS_CC))
      return;

    rc = cass_ssl_set_private_key(ssl->ssl, contents, builder->passphrase);
    efree(contents);
    ASSERT_SUCCESS(rc);
  }
}

PHP_METHOD(SSLOptionsBuilder, withTrustedCerts)
{
  zval*** args;
  zval*   path;
  zval    readable;
  int     argc, i;
  cassandra_ssl_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
    return;
  }

  for (i = 0; i < argc; i++) {
    path = *args[i];

    if (Z_TYPE_P(path) != IS_STRING) {
      throw_invalid_argument(path, "path", "a path to a trusted cert file" TSRMLS_CC);
      efree(args);
    }

    php_stat(Z_STRVAL_P(path), Z_STRLEN_P(path), FS_IS_R, &readable TSRMLS_CC);

    if (!Z_LVAL(readable)) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "The path '%s' doesn't exist or is not readable", Z_STRVAL_P(path));
      efree(args);
      return;
    }
  }

  builder = (cassandra_ssl_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (builder->trusted_certs) {
    for (i = 0; i < builder->trusted_certs_cnt; i++)
      efree(builder->trusted_certs[i]);

    efree(builder->trusted_certs);
  }

  builder->trusted_certs_cnt = argc;
  builder->trusted_certs     = ecalloc(argc, sizeof(char*));

  for (i = 0; i < argc; i++) {
    path = *args[i];

    builder->trusted_certs[i] = estrndup(Z_STRVAL_P(path), Z_STRLEN_P(path));
  }
  efree(args);

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(SSLOptionsBuilder, withVerifyFlags)
{
  long flags;
  cassandra_ssl_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flags) == FAILURE) {
    return;
  }

  builder = (cassandra_ssl_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  builder->flags = (int) flags;

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(SSLOptionsBuilder, withClientCert)
{
  char* client_cert;
  int   client_cert_len;
  zval  readable;
  cassandra_ssl_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &client_cert, &client_cert_len) == FAILURE) {
    return;
  }

  php_stat(client_cert, client_cert_len, FS_IS_R, &readable TSRMLS_CC);

  if (!Z_LVAL(readable)) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
      "The path '%s' doesn't exist or is not readable", client_cert);
    return;
  }

  builder = (cassandra_ssl_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (builder->client_cert)
    efree(builder->client_cert);

  builder->client_cert = estrndup(client_cert, client_cert_len);

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(SSLOptionsBuilder, withPrivateKey)
{
  char* private_key;
  char* passphrase = NULL;
  int   private_key_len, passphrase_len;
  zval  readable;
  cassandra_ssl_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &private_key, &private_key_len, &passphrase, &passphrase_len) == FAILURE) {
    return;
  }

  php_stat(private_key, private_key_len, FS_IS_R, &readable TSRMLS_CC);

  if (!Z_LVAL(readable)) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
      "The path '%s' doesn't exist or is not readable", private_key);
    return;
  }

  builder = (cassandra_ssl_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (builder->private_key)
    efree(builder->private_key);

  builder->private_key = estrndup(private_key, private_key_len);

  if (builder->passphrase) {
    efree(builder->passphrase);
    builder->passphrase = NULL;
  }

  if (passphrase)
    builder->passphrase = estrndup(passphrase, passphrase_len);

  RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_path, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_flags, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_key, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, path)
  ZEND_ARG_INFO(0, passphrase)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_ssl_builder_methods[] = {
  PHP_ME(SSLOptionsBuilder, build, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(SSLOptionsBuilder, withTrustedCerts, arginfo_path,
    ZEND_ACC_PUBLIC)
  PHP_ME(SSLOptionsBuilder, withVerifyFlags, arginfo_flags,
    ZEND_ACC_PUBLIC)
  PHP_ME(SSLOptionsBuilder, withClientCert, arginfo_path,
    ZEND_ACC_PUBLIC)
  PHP_ME(SSLOptionsBuilder, withPrivateKey, arginfo_key,
    ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_ssl_builder_handlers;

static HashTable*
php_cassandra_ssl_builder_properties(zval *object TSRMLS_DC)
{
  /* cassandra_ssl_builder* self = (cassandra_ssl_builder*) zend_object_store_get_object(object TSRMLS_CC); */
  HashTable* props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_ssl_builder_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_ssl_builder_free(void *object TSRMLS_DC)
{
  cassandra_ssl_builder* builder = (cassandra_ssl_builder*) object;

  zend_object_std_dtor(&builder->zval TSRMLS_CC);

  if (builder->trusted_certs) {
    int i;

    for (i = 0; i < builder->trusted_certs_cnt; i++)
      efree(builder->trusted_certs[i]);

    efree(builder->trusted_certs);
  }

  if (builder->client_cert)
    efree(builder->client_cert);

  if (builder->private_key)
    efree(builder->private_key);

  if (builder->passphrase)
    efree(builder->passphrase);

  efree(builder);
}

static zend_object_value
php_cassandra_ssl_builder_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_ssl_builder *builder;

  builder = (cassandra_ssl_builder*) ecalloc(1, sizeof(cassandra_ssl_builder));

  zend_object_std_init(&builder->zval, class_type TSRMLS_CC);
  object_properties_init(&builder->zval, class_type);

  builder->flags             = 0;
  builder->trusted_certs     = NULL;
  builder->trusted_certs_cnt = 0;
  builder->client_cert       = NULL;
  builder->private_key       = NULL;
  builder->passphrase        = NULL;

  retval.handle   = zend_objects_store_put(builder,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_ssl_builder_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_ssl_builder_handlers;

  return retval;
}

void cassandra_define_SSLOptionsBuilder(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\SSLOptions\\Builder", cassandra_ssl_builder_methods);
  cassandra_ssl_builder_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ssl_builder_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_ssl_builder_ce->create_object = php_cassandra_ssl_builder_new;

  memcpy(&cassandra_ssl_builder_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_ssl_builder_handlers.get_properties  = php_cassandra_ssl_builder_properties;
  cassandra_ssl_builder_handlers.compare_objects = php_cassandra_ssl_builder_compare;
}
