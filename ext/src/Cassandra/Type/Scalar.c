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
  cassandra_type_scalar *self;
  const char *name;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE_SCALAR(getThis());
  name = php_cassandra_scalar_type_name(self->type TSRMLS_CC);
  PHP5TO7_RETURN_STRING(name);
}

PHP_METHOD(TypeScalar, __toString)
{
  cassandra_type_scalar *self;
  const char *name;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE_SCALAR(getThis());
  name = php_cassandra_scalar_type_name(self->type TSRMLS_CC);
  PHP5TO7_RETURN_STRING(name);
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
php_cassandra_type_scalar_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_type_scalar *self = (cassandra_type_scalar *) object;

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_ZEND_OBJECT_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_type_scalar_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_type_scalar *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(type_scalar, ce);

  self->type = CASS_VALUE_TYPE_UNKNOWN;

  PHP5TO7_ZEND_OBJECT_INIT(type_scalar, self, ce);
}

void cassandra_define_TypeScalar(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Scalar", cassandra_type_scalar_methods);
  cassandra_type_scalar_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_scalar_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_scalar_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_type_scalar_ce->create_object = php_cassandra_type_scalar_new;

  memcpy(&cassandra_type_scalar_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
