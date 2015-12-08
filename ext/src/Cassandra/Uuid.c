#include "php_cassandra.h"
#include "util/types.h"
#include "util/uuid_gen.h"

zend_class_entry *cassandra_uuid_ce = NULL;

void
php_cassandra_uuid_init(INTERNAL_FUNCTION_PARAMETERS)
{
  char *value;
  int value_len;
  cassandra_uuid* self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &value, &value_len) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_uuid_ce TSRMLS_CC)) {
    self = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);
  } else {
    object_init_ex(return_value, cassandra_uuid_ce);
    self = (cassandra_uuid*) zend_object_store_get_object(return_value TSRMLS_CC);
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
  cassandra_uuid* self   = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);
  char*           string = emalloc((CASS_UUID_STRING_LENGTH) * sizeof(char));

  cass_uuid_string(self->uuid, string);

  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Uuid::type() */
PHP_METHOD(Uuid, type)
{
  cassandra_uuid* self = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->type, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Uuid::value() */
PHP_METHOD(Uuid, uuid)
{
  cassandra_uuid* self   = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);
  char*           string = emalloc((CASS_UUID_STRING_LENGTH) * sizeof(char));

  cass_uuid_string(self->uuid, string);

  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Uuid::version() */
PHP_METHOD(Uuid, version)
{
  cassandra_uuid* self = (cassandra_uuid*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_LONG((long) cass_uuid_version(self->uuid));
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
  PHP_ME(Uuid, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Uuid, uuid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Uuid, version, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_uuid_handlers;

static HashTable*
php_cassandra_uuid_gc(zval *object, zval ***table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_uuid_properties(zval *object TSRMLS_DC)
{
  cassandra_uuid* self  = (cassandra_uuid*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*      props = zend_std_get_properties(object TSRMLS_CC);

  zval* uuid_str;
  zval* version;

  char* string = emalloc((CASS_UUID_STRING_LENGTH) * sizeof(char));

  cass_uuid_string(self->uuid, string);

  MAKE_STD_ZVAL(uuid_str);
  ZVAL_STRING(uuid_str, string, 0);
  MAKE_STD_ZVAL(version);
  ZVAL_LONG(version, (long) cass_uuid_version(self->uuid));

  zend_hash_update(props, "uuid", sizeof("uuid"), &uuid_str, sizeof(zval), NULL);
  zend_hash_update(props, "version", sizeof("version"), &version, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_uuid_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_uuid* uuid1 = NULL;
  cassandra_uuid* uuid2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  uuid1 = (cassandra_uuid*) zend_object_store_get_object(obj1 TSRMLS_CC);
  uuid2 = (cassandra_uuid*) zend_object_store_get_object(obj2 TSRMLS_CC);

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
  cassandra_uuid* self = (cassandra_uuid*) object;

  zval_ptr_dtor(&self->type);
  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_uuid_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_uuid* self;

  self = (cassandra_uuid*) emalloc(sizeof(cassandra_uuid));
  memset(self, 0, sizeof(cassandra_uuid));

  self->type = php_cassandra_type_scalar(CASS_VALUE_TYPE_UUID TSRMLS_CC);
  Z_ADDREF_P(self->type);

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  retval.handle   = zend_objects_store_put(self, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_uuid_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_uuid_handlers;

  return retval;
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
  cassandra_uuid_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_uuid_ce->create_object = php_cassandra_uuid_new;
}
