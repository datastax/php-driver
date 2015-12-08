#include "php_cassandra.h"
#include "util/types.h"

zend_class_entry *cassandra_type_custom_ce = NULL;

PHP_METHOD(TypeCustom, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
    "Instantiation of a Cassandra\\Type\\Custom type is not supported."
  );
  return;
}

PHP_METHOD(TypeCustom, name)
{
  cassandra_type_custom* self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_custom*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_STRING(self->name, 1);
}

PHP_METHOD(TypeCustom, __toString)
{
  cassandra_type_custom* self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_custom*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_STRING(self->name, 1);
}

PHP_METHOD(TypeCustom, create)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
    "Instantiation of a Cassandra\\Type\\Custom instance is not supported."
  );
  return;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_type_custom_methods[] = {
  PHP_ME(TypeCustom, name,       arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(TypeCustom, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(TypeCustom, create,     arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_type_custom_handlers;

static void
php_cassandra_type_custom_free(void *object TSRMLS_DC)
{
  cassandra_type_custom* self = (cassandra_type_custom*) object;

  if (self->name) {
    efree(self->name);
    self->name = NULL;
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_type_custom_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_type_custom* self;

  self = (cassandra_type_custom*) ecalloc(1, sizeof(cassandra_type_custom));
  memset(self, 0, sizeof(cassandra_type_custom));

  self->type = CASS_VALUE_TYPE_CUSTOM;
  self->name = NULL;

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  retval.handle   = zend_objects_store_put(self,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_type_custom_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_type_custom_handlers;

  return retval;
}

void cassandra_define_TypeCustom(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Custom", cassandra_type_custom_methods);
  cassandra_type_custom_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_custom_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_custom_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_type_custom_ce->create_object = php_cassandra_type_custom_new;

  memcpy(&cassandra_type_custom_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
