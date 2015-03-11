#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "util/uuid_gen.h"
#include "uuid.h"

extern zend_class_entry *cassandra_ce_InvalidArgumentException;

zend_class_entry *cassandra_ce_Uuid = NULL;

/* {{{ Cassandra\Uuid::__construct(string) */
PHP_METHOD(CassandraUuid, __construct)
{
  char *value;
  int value_len;
  cassandra_uuid* uuid;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &value, &value_len) == FAILURE) {
    return;
  }

  uuid = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (ZEND_NUM_ARGS() == 0)
    php_cassandra_uuid_generate_random(&uuid->uuid);
  else {
    char* str = strndup(value, value_len);
    int rc = cass_uuid_from_string(str, &uuid->uuid);

    if (rc != CASS_OK) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid uuid value: \"%s\"", str);
      free(str);
      return;
    }

    free(str);
  }

  char* string = emalloc((CASS_UUID_STRING_LENGTH) * sizeof(char));
  cass_uuid_string(uuid->uuid, string);

  zend_update_property_stringl(cassandra_ce_Uuid, getThis(), "uuid", strlen("uuid"), string, CASS_UUID_STRING_LENGTH - 1 TSRMLS_CC);
  zend_update_property_long(cassandra_ce_Uuid, getThis(), "version", strlen("version"), (long) cass_uuid_version(uuid->uuid) TSRMLS_CC);

  efree(string);
}
/* }}} */

/* {{{ Cassandra\Uuid::__toString() */
PHP_METHOD(CassandraUuid, __toString)
{
  zval *zode = zend_read_property(cassandra_ce_Uuid, getThis(), "uuid", strlen("uuid"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

/* {{{ Cassandra\Uuid::value() */
PHP_METHOD(CassandraUuid, uuid)
{
  zval *zode = zend_read_property(cassandra_ce_Uuid, getThis(), "uuid", strlen("uuid"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

/* {{{ Cassandra\Uuid::value() */
PHP_METHOD(CassandraUuid, version)
{
  zval *zode = zend_read_property(cassandra_ce_Uuid, getThis(), "version", strlen("version"), 0 TSRMLS_CC);

  RETURN_LONG(Z_LVAL_P(zode));
}
/* }}} */

static zend_function_entry CassandraUuid_methods[] = {
  PHP_ME(CassandraUuid, __construct, NULL, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraUuid, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraUuid, uuid, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraUuid, version, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static void
php_cassandra_uuid_free(void *object TSRMLS_DC)
{
  cassandra_uuid* uuid = (cassandra_uuid*) object;

  zend_object_std_dtor(&uuid->zval TSRMLS_CC);

  efree(uuid);
}

static zend_object_value
php_cassandra_uuid_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_uuid *uuid;

  uuid = (cassandra_uuid*) emalloc(sizeof(cassandra_uuid));
  memset(uuid, 0, sizeof(cassandra_uuid));

  zend_object_std_init(&uuid->zval, class_type TSRMLS_CC);
  object_properties_init(&uuid->zval, class_type TSRMLS_CC);

  retval.handle = zend_objects_store_put(uuid, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_uuid_free, NULL TSRMLS_CC);
  retval.handlers = zend_get_std_object_handlers();

  return retval;
}

void
cassandra_define_CassandraUuid(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Uuid", CassandraUuid_methods);
  cassandra_ce_Uuid = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_Uuid->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Uuid->create_object = php_cassandra_uuid_new;

  /* fields */
  zend_declare_property_string(cassandra_ce_Uuid, "uuid", strlen("uuid"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
  zend_declare_property_long(cassandra_ce_Uuid, "version", strlen("version"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
}
