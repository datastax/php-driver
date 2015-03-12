#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "util/uuid_gen.h"
#include "uuid_interface.h"
#include "timeuuid.h"
#include <ext/date/php_date.h>

extern zend_class_entry* cassandra_ce_InvalidArgumentException;
extern zend_class_entry* cassandra_ce_UuidInterface;

zend_class_entry *cassandra_ce_Timeuuid = NULL;

/* {{{ Cassandra\Timeuuid::__construct(string) */
PHP_METHOD(CassandraTimeuuid, __construct)
{
  long timestamp;
  cassandra_uuid* uuid;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timestamp) == FAILURE) {
    return;
  }

  uuid = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (ZEND_NUM_ARGS() == 0) {
    php_cassandra_uuid_generate_time(&uuid->uuid);
  } else {
    if (timestamp < 0) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Timestamp must be a positive integer, \"%d\" given", timestamp);
      return;
    }
    php_cassandra_uuid_generate_from_time(timestamp, &uuid->uuid);
  }

  char* string = emalloc((CASS_UUID_STRING_LENGTH) * sizeof(char));
  cass_uuid_string(uuid->uuid, string);

  zend_update_property_stringl(cassandra_ce_Timeuuid, getThis(), "uuid", strlen("uuid"), string, CASS_UUID_STRING_LENGTH - 1 TSRMLS_CC);
  zend_update_property_long(cassandra_ce_Timeuuid, getThis(), "version", strlen("version"), (long) cass_uuid_version(uuid->uuid) TSRMLS_CC);

  efree(string);
}
/* }}} */

/* {{{ Cassandra\Timeuuid::__toString() */
PHP_METHOD(CassandraTimeuuid, __toString)
{
  zval *zode = zend_read_property(cassandra_ce_Timeuuid, getThis(), "uuid", strlen("uuid"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

/* {{{ Cassandra\Timeuuid::value() */
PHP_METHOD(CassandraTimeuuid, uuid)
{
  zval *zode = zend_read_property(cassandra_ce_Timeuuid, getThis(), "uuid", strlen("uuid"), 0 TSRMLS_CC);

  RETURN_STRING(Z_STRVAL_P(zode), 1);
}
/* }}} */

/* {{{ Cassandra\Timeuuid::value() */
PHP_METHOD(CassandraTimeuuid, version)
{
  zval *zode = zend_read_property(cassandra_ce_Timeuuid, getThis(), "version", strlen("version"), 0 TSRMLS_CC);

  RETURN_LONG(Z_LVAL_P(zode));
}
/* }}} */

/* {{{ Cassandra\Timeuuid::value() */
PHP_METHOD(CassandraTimeuuid, time)
{
  cassandra_uuid* uuid;

  uuid = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG((long) (cass_uuid_timestamp(uuid->uuid) / 1000));
}
/* }}} */

/* {{{ Cassandra\Timeuuid::value() */
PHP_METHOD(CassandraTimeuuid, toDateTime)
{
  cassandra_uuid* uuid;
  zval* datetime;
  php_date_obj* datetime_obj;
  char* str;
  int str_len;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  uuid = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);

  MAKE_STD_ZVAL(datetime);
  php_date_instantiate(php_date_get_date_ce(), datetime TSRMLS_CC);

  datetime_obj = zend_object_store_get_object(datetime TSRMLS_CC);
  str_len      = spprintf(&str, 0, "@%ld", (long) (cass_uuid_timestamp(uuid->uuid) / 1000));
  php_date_initialize(datetime_obj, str, str_len, NULL, NULL, 0 TSRMLS_CC);
  efree(str);

  RETVAL_ZVAL(datetime, 0, 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static zend_function_entry CassandraTimeuuid_methods[] = {
  PHP_ME(CassandraTimeuuid, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimeuuid, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimeuuid, uuid, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimeuuid, version, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimeuuid, time, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimeuuid, toDateTime, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static void
php_cassandra_timeuuid_free(void *object TSRMLS_DC)
{
  cassandra_uuid* uuid = (cassandra_uuid*) object;

  zend_object_std_dtor(&uuid->zval TSRMLS_CC);

  efree(uuid);
}

static zend_object_value
php_cassandra_timeuuid_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_uuid *uuid;

  uuid = (cassandra_uuid*) emalloc(sizeof(cassandra_uuid));
  memset(uuid, 0, sizeof(cassandra_uuid));

  zend_object_std_init(&uuid->zval, class_type TSRMLS_CC);
  object_properties_init(&uuid->zval, class_type TSRMLS_CC);

  retval.handle = zend_objects_store_put(uuid, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_timeuuid_free, NULL TSRMLS_CC);
  retval.handlers = zend_get_std_object_handlers();

  return retval;
}

void
cassandra_define_CassandraTimeuuid(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Timeuuid", CassandraTimeuuid_methods);
  cassandra_ce_Timeuuid = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_ce_Timeuuid TSRMLS_CC, 1, cassandra_ce_UuidInterface);
  cassandra_ce_Timeuuid->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Timeuuid->create_object = php_cassandra_timeuuid_new;

  /* fields */
  zend_declare_property_string(cassandra_ce_Timeuuid, "uuid", strlen("uuid"), "", ZEND_ACC_PRIVATE TSRMLS_CC);
  zend_declare_property_long(cassandra_ce_Timeuuid, "version", strlen("version"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);
}
