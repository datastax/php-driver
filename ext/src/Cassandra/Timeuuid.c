#include "php_cassandra.h"
#include "util/uuid_gen.h"
#include <ext/date/php_date.h>

zend_class_entry *cassandra_timeuuid_ce = NULL;

void
php_cassandra_timeuuid_init(INTERNAL_FUNCTION_PARAMETERS)
{
  long timestamp;
  cassandra_uuid *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timestamp) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_timeuuid_ce TSRMLS_CC)) {
    self = PHP_CASSANDRA_GET_UUID(getThis());
  } else {
    object_init_ex(return_value, cassandra_timeuuid_ce);
    self = PHP_CASSANDRA_GET_UUID(return_value);
  }

  if (ZEND_NUM_ARGS() == 0) {
    php_cassandra_uuid_generate_time(&self->uuid TSRMLS_CC);
  } else {
    if (timestamp < 0) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC, "Timestamp must be a positive integer, %d given", timestamp);
      return;
    }
    php_cassandra_uuid_generate_from_time(timestamp, &self->uuid TSRMLS_CC);
  }
}

/* {{{ Cassandra\Timeuuid::__construct(string) */
PHP_METHOD(Timeuuid, __construct)
{
  php_cassandra_timeuuid_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Timeuuid::__toString() */
PHP_METHOD(Timeuuid, __toString)
{
  char string[CASS_UUID_STRING_LENGTH];
  cassandra_uuid *self = PHP_CASSANDRA_GET_UUID(getThis());

  cass_uuid_string(self->uuid, string);

  PHP5TO7_RETVAL_STRING(string);
}
/* }}} */

/* {{{ Cassandra\Timeuuid::value() */
PHP_METHOD(Timeuuid, uuid)
{
  char string[CASS_UUID_STRING_LENGTH];
  cassandra_uuid *self = PHP_CASSANDRA_GET_UUID(getThis());

  cass_uuid_string(self->uuid, string);

  PHP5TO7_RETVAL_STRING(string);
}
/* }}} */

/* {{{ Cassandra\Timeuuid::value() */
PHP_METHOD(Timeuuid, version)
{
  cassandra_uuid *self = PHP_CASSANDRA_GET_UUID(getThis());

  RETURN_LONG((long) cass_uuid_version(self->uuid));
}
/* }}} */

/* {{{ Cassandra\Timeuuid::value() */
PHP_METHOD(Timeuuid, time)
{
  cassandra_uuid *self = PHP_CASSANDRA_GET_UUID(getThis());
  RETURN_LONG((long) (cass_uuid_timestamp(self->uuid) / 1000));
}
/* }}} */

/* {{{ Cassandra\Timeuuid::value() */
PHP_METHOD(Timeuuid, toDateTime)
{
  cassandra_uuid *self;
  zval *datetime = NULL;
  php_date_obj *datetime_obj = NULL;
  char *str;
  int str_len;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_UUID(getThis());

  PHP5TO7_ZVAL_MAYBE_MAKE(datetime);
  php_date_instantiate(php_date_get_date_ce(), datetime TSRMLS_CC);

#if PHP_MAJOR_VERSION >= 7
  datetime_obj = php_date_obj_from_obj(Z_OBJ_P(datetime));
#else
  datetime_obj = zend_object_store_get_object(datetime TSRMLS_CC);
#endif

  str_len      = spprintf(&str, 0, "@%ld", (long) (cass_uuid_timestamp(self->uuid) / 1000));
  php_date_initialize(datetime_obj, str, str_len, NULL, NULL, 0 TSRMLS_CC);
  efree(str);

  RETVAL_ZVAL(datetime, 0, 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_timeuuid_methods[] = {
  PHP_ME(Timeuuid, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, uuid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, version, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, time, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, toDateTime, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_timeuuid_handlers;

static HashTable *
php_cassandra_timeuuid_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_cassandra_timeuuid_properties(zval *object TSRMLS_DC)
{
  char string[CASS_UUID_STRING_LENGTH];

  cassandra_uuid *self = PHP_CASSANDRA_GET_UUID(object);
  HashTable      *props = zend_std_get_properties(object TSRMLS_CC);
  php5to7_zval    uuid;
  php5to7_zval    version;

  cass_uuid_string(self->uuid, string);

  PHP5TO7_ZVAL_MAYBE_MAKE(uuid);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(uuid), string);

  PHP5TO7_ZVAL_MAYBE_MAKE(version);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(version), (long) cass_uuid_version(self->uuid));

  PHP5TO7_ZEND_HASH_UPDATE(props, "uuid", sizeof("uuid"), PHP5TO7_ZVAL_MAYBE_P(uuid), sizeof(zval));
  PHP5TO7_ZEND_HASH_UPDATE(props, "version", sizeof("version"), PHP5TO7_ZVAL_MAYBE_P(version), sizeof(zval));

  return props;
}

static int
php_cassandra_timeuuid_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_uuid *uuid1 = NULL;
  cassandra_uuid *uuid2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  uuid1 = PHP_CASSANDRA_GET_UUID(obj1);
  uuid2 = PHP_CASSANDRA_GET_UUID(obj2);

  if (uuid1->uuid.time_and_version == uuid2->uuid.time_and_version) {
    if (uuid1->uuid.clock_seq_and_node == uuid2->uuid.clock_seq_and_node)
      return 0;
    else if (uuid1->uuid.clock_seq_and_node < uuid2->uuid.clock_seq_and_node)
      return -1;
    else
      return 1;
  } else if (uuid1->uuid.time_and_version < uuid2->uuid.time_and_version) {
    return -1;
  } else {
    return 1;
  }
}

static void
php_cassandra_timeuuid_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_uuid *self = PHP5TO7_ZEND_OBJECT_GET(uuid, object);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_timeuuid_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_uuid *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(uuid, ce);

  PHP5TO7_ZEND_OBJECT_INIT_EX(uuid, timeuuid, self, ce);
}

void
cassandra_define_Timeuuid(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Timeuuid", cassandra_timeuuid_methods);
  cassandra_timeuuid_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_timeuuid_ce TSRMLS_CC, 1, cassandra_uuid_interface_ce);
  memcpy(&cassandra_timeuuid_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_timeuuid_handlers.get_properties  = php_cassandra_timeuuid_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_timeuuid_handlers.get_gc          = php_cassandra_timeuuid_gc;
#endif
  cassandra_timeuuid_handlers.compare_objects = php_cassandra_timeuuid_compare;
  cassandra_timeuuid_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_timeuuid_ce->create_object = php_cassandra_timeuuid_new;
}
