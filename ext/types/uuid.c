#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "util/uuid_gen.h"
#include "uuid_interface.h"
#include "uuid.h"

extern zend_class_entry* cassandra_ce_InvalidArgumentException;
extern zend_class_entry* cassandra_ce_UuidInterface;

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
    if (cass_uuid_from_string(value, &uuid->uuid) != CASS_OK) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid uuid value: \"%s\"", value);
      return;
    }
  }
}
/* }}} */

/* {{{ Cassandra\Uuid::__toString() */
PHP_METHOD(CassandraUuid, __toString)
{
  cassandra_uuid* uuid   = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);
  char*           string = emalloc((CASS_UUID_STRING_LENGTH) * sizeof(char));

  cass_uuid_string(uuid->uuid, string);

  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Uuid::value() */
PHP_METHOD(CassandraUuid, uuid)
{
  cassandra_uuid* uuid   = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);
  char*           string = emalloc((CASS_UUID_STRING_LENGTH) * sizeof(char));

  cass_uuid_string(uuid->uuid, string);

  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Uuid::value() */
PHP_METHOD(CassandraUuid, version)
{
  cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_LONG((long) cass_uuid_version(uuid->uuid));
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, uuid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry CassandraUuid_methods[] = {
  PHP_ME(CassandraUuid, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraUuid, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraUuid, uuid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraUuid, version, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_uuid_handlers;

static HashTable*
php_cassandra_uuid_properties(zval *object TSRMLS_DC)
{
  cassandra_uuid* uuid  = (cassandra_uuid*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*      props = zend_std_get_properties(object TSRMLS_CC);

  zval* uuid_str;
  zval* version;

  char* string = emalloc((CASS_UUID_STRING_LENGTH) * sizeof(char));

  cass_uuid_string(uuid->uuid, string);

  MAKE_STD_ZVAL(uuid_str);
  ZVAL_STRING(uuid_str, string, 0);
  MAKE_STD_ZVAL(version);
  ZVAL_LONG(version, (long) cass_uuid_version(uuid->uuid));

  zend_hash_update(props, "uuid", sizeof("uuid"), &uuid_str, sizeof(zval), NULL);
  zend_hash_update(props, "version", sizeof("version"), &version, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_uuid_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_uuid* uuid1 = (cassandra_uuid*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_uuid* uuid2 = (cassandra_uuid*) zend_object_store_get_object(obj2 TSRMLS_CC);

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

  retval.handle   = zend_objects_store_put(uuid, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_uuid_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_uuid_handlers;

  return retval;
}

void
cassandra_define_CassandraUuid(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Uuid", CassandraUuid_methods);
  cassandra_ce_Uuid = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_ce_Uuid TSRMLS_CC, 1, cassandra_ce_UuidInterface);
  memcpy(&cassandra_uuid_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_uuid_handlers.get_properties = php_cassandra_uuid_properties;
  cassandra_uuid_handlers.compare_objects = php_cassandra_uuid_compare;
  cassandra_ce_Uuid->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Uuid->create_object = php_cassandra_uuid_new;
}
