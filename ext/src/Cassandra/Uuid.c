#include "php_cassandra.h"
#include "util/uuid_gen.h"

zend_class_entry *cassandra_uuid_ce = NULL;

void
php_cassandra_uuid_init(INTERNAL_FUNCTION_PARAMETERS)
{
  char *value;
  int value_len;
  cassandra_uuid *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &value, &value_len) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_uuid_ce TSRMLS_CC)) {
    self = PHP_CASSANDRA_GET_UUID(getThis());
  } else {
    object_init_ex(return_value, cassandra_uuid_ce);
    self = PHP_CASSANDRA_GET_UUID(return_value);
  }

  if (ZEND_NUM_ARGS() == 0) {
    php_cassandra_uuid_generate_random(&self->uuid TSRMLS_CC);
  } else {
    if (cass_uuid_from_string(value, &self->uuid) != CASS_OK) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "Invalid UUID: '%.*s'", value_len, value);
      return;
    }
  }
}

/* {{{ Cassandra\Uuid::__construct(string) */
PHP_METHOD(Uuid, __construct)
{
  php_cassandra_uuid_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Uuid::__toString() */
PHP_METHOD(Uuid, __toString)
{
  char string[CASS_UUID_STRING_LENGTH];
  cassandra_uuid *uuid = PHP_CASSANDRA_GET_UUID(getThis());

  cass_uuid_string(uuid->uuid, string);

  PHP5TO7_RETURN_STRING(string);
}
/* }}} */

/* {{{ Cassandra\Uuid::value() */
PHP_METHOD(Uuid, uuid)
{
  char string[CASS_UUID_STRING_LENGTH];
  cassandra_uuid *uuid = PHP_CASSANDRA_GET_UUID(getThis());

  cass_uuid_string(uuid->uuid, string);

  PHP5TO7_RETURN_STRING(string);
}
/* }}} */

/* {{{ Cassandra\Uuid::value() */
PHP_METHOD(Uuid, version)
{
  cassandra_uuid *uuid = PHP_CASSANDRA_GET_UUID(getThis());

  RETURN_LONG((long) cass_uuid_version(uuid->uuid));
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, uuid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_uuid_methods[] = {
  PHP_ME(Uuid, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Uuid, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Uuid, uuid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Uuid, version, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_uuid_handlers;

static HashTable *
php_cassandra_uuid_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_cassandra_uuid_properties(zval *object TSRMLS_DC)
{
  char string[CASS_UUID_STRING_LENGTH];

  cassandra_uuid *uuid  = PHP_CASSANDRA_GET_UUID(object);
  HashTable      *props = zend_std_get_properties(object TSRMLS_CC);
  zval           *uuid_str = NULL;
  zval           *version  = NULL;

  cass_uuid_string(uuid->uuid, string);

  PHP5TO7_ZVAL_MAYBE_MAKE(uuid_str);
  PHP5TO7_ZVAL_STRING(uuid_str, string);

  PHP5TO7_ZVAL_MAYBE_MAKE(version);
  ZVAL_LONG(version, (long) cass_uuid_version(uuid->uuid));

  PHP5TO7_ZEND_HASH_UPDATE(props, "uuid", sizeof("uuid"), uuid_str, sizeof(zval));
  PHP5TO7_ZEND_HASH_UPDATE(props, "version", sizeof("version"), version, sizeof(zval));

  return props;
}

static int
php_cassandra_uuid_compare(zval *obj1, zval *obj2 TSRMLS_DC)
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
php_cassandra_uuid_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_uuid *self = (cassandra_uuid *) object;

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_ZEND_OBJECT_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_uuid_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_uuid *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(uuid, ce);

  PHP5TO7_ZEND_OBJECT_INIT(uuid, self, ce);
}

void
cassandra_define_Uuid(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Uuid", cassandra_uuid_methods);
  cassandra_uuid_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_uuid_ce TSRMLS_CC, 1, cassandra_uuid_interface_ce);
  memcpy(&cassandra_uuid_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_uuid_handlers.get_properties  = php_cassandra_uuid_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_uuid_handlers.get_gc          = php_cassandra_uuid_gc;
#endif
  cassandra_uuid_handlers.compare_objects = php_cassandra_uuid_compare;
  cassandra_uuid_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_uuid_ce->create_object = php_cassandra_uuid_new;
}
