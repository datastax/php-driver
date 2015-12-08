#include "php_cassandra.h"
#include "util/types.h"

zend_class_entry *cassandra_type_scalar_ce = NULL;

PHP_METHOD(TypeScalar, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
    "Instantiation of a Cassandra\\Type\\Scalar objects directly is not " \
    "supported, call varchar(), text(), blob(), ascii(), bigint(), " \
    "counter(), int(), varint(), boolean(), decimal(), double(), float(), " \
    "inet(), timestamp(), uuid(), timeuuid(), map(), collection() or set() " \
    "on Cassandra\\Type statically instead."
  );
  return;
}

PHP_METHOD(TypeScalar, name)
{
  cassandra_type_scalar* self;
  const char* name;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_scalar*) zend_object_store_get_object(getThis() TSRMLS_CC);
  name = php_cassandra_scalar_type_name(self->type TSRMLS_CC);
  RETURN_STRING(name, 1);
}

PHP_METHOD(TypeScalar, __toString)
{
  cassandra_type_scalar* self;
  const char* name;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_scalar*) zend_object_store_get_object(getThis() TSRMLS_CC);
  name = php_cassandra_scalar_type_name(self->type TSRMLS_CC);
  RETURN_STRING(name, 1);
}

PHP_METHOD(TypeScalar, create)
{
  php_cassandra_scalar_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_type_scalar_methods[] = {
  PHP_ME(TypeScalar, name,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(TypeScalar, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(TypeScalar, create,     arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_type_scalar_handlers;

static void
php_cassandra_type_scalar_free(void *object TSRMLS_DC)
{
  cassandra_type_scalar* self = (cassandra_type_scalar*) object;

  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_type_scalar_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_type_scalar* self;

  self = (cassandra_type_scalar*) ecalloc(1, sizeof(cassandra_type_scalar));
  memset(self, 0, sizeof(cassandra_type_scalar));

  self->type = CASS_VALUE_TYPE_UNKNOWN;

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  retval.handle   = zend_objects_store_put(self,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_type_scalar_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_type_scalar_handlers;

  return retval;
}

void cassandra_define_TypeScalar(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Scalar", cassandra_type_scalar_methods);
  cassandra_type_scalar_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_scalar_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_scalar_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_type_scalar_ce->create_object = php_cassandra_type_scalar_new;

  memcpy(&cassandra_type_scalar_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
