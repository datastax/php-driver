#include "php_cassandra.h"
#include "util/types.h"
#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif
#include "src/Cassandra/Set.h"

zend_class_entry *cassandra_type_set_ce = NULL;

PHP_METHOD(TypeSet, name)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  PHP5TO7_RETVAL_STRING("set");
}

PHP_METHOD(TypeSet, valueType)
{
  cassandra_type *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->value_type), 1, 0);
}

PHP_METHOD(TypeSet, __toString)
{
  cassandra_type *self;
  smart_str string = PHP5TO7_SMART_STR_INIT;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE(getThis());

  php_cassandra_type_string(self, &string TSRMLS_CC);
  smart_str_0(&string);

  PHP5TO7_RETVAL_STRING(PHP5TO7_SMART_STR_VAL(string));
  smart_str_free(&string);
}

PHP_METHOD(TypeSet, create)
{
  cassandra_type *self;
  cassandra_set *set;
  php5to7_zval_args args = NULL;
  int argc = 0, i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*",
                            &args, &argc) == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE(getThis());

  object_init_ex(return_value, cassandra_set_ce);
  set = PHP_CASSANDRA_GET_SET(return_value);

  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(set->type), getThis());

  if (argc > 0) {
    for (i = 0; i < argc; i++) {
      if (!php_cassandra_set_add(set, PHP5TO7_ZVAL_ARG(args[i]) TSRMLS_CC)) {
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
    }

    PHP5TO7_MAYBE_EFREE(args);
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_type_set_methods[] = {
  PHP_ME(TypeSet, name,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeSet, valueType,  arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeSet, __toString, arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeSet, create,     arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_type_set_handlers;

static void
php_cassandra_type_set_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_type *self = PHP5TO7_ZEND_OBJECT_GET(type, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->value_type);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_type_set_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_type *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(type, ce);

  self->type = CASS_VALUE_TYPE_SET;
  PHP5TO7_ZVAL_UNDEF(self->value_type);

  PHP5TO7_ZEND_OBJECT_INIT_EX(type, type_set, self, ce);
}

void cassandra_define_TypeSet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Set", cassandra_type_set_methods);
  cassandra_type_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_set_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_set_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_type_set_ce->create_object = php_cassandra_type_set_new;

  memcpy(&cassandra_type_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
