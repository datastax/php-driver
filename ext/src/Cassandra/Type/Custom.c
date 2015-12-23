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
  cassandra_type_custom *custom;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  custom = PHP_CASSANDRA_GET_TYPE_CUSTOM(getThis());

  PHP5TO7_RETURN_STRING(custom->name);
}

PHP_METHOD(TypeCustom, __toString)
{
  cassandra_type_custom *custom;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  custom = PHP_CASSANDRA_GET_TYPE_CUSTOM(getThis());

  PHP5TO7_RETURN_STRING(custom->name);
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
php_cassandra_type_custom_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_type_custom *self = (cassandra_type_custom *) object;

  if (self->name) {
    efree(self->name);
    self->name = NULL;
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_ZEND_OBJECT_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_type_custom_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_type_custom *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(type_custom, ce);

  self->name = NULL;

  PHP5TO7_ZEND_OBJECT_INIT(type_custom, self, ce);
}

void cassandra_define_TypeCustom(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Custom", cassandra_type_custom_methods);
  cassandra_type_custom_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_custom_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_custom_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_type_custom_ce->create_object = php_cassandra_type_custom_new;

  memcpy(&cassandra_type_custom_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
