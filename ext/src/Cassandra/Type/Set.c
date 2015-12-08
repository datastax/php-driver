#include "php_cassandra.h"
#include "util/types.h"
#include "src/Cassandra/Set.h"

zend_class_entry *cassandra_type_set_ce = NULL;

PHP_METHOD(TypeSet, name)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  RETURN_STRING("set", 1);
}

PHP_METHOD(TypeSet, type)
{
  cassandra_type_set* self;
  zval* type;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  type = php_cassandra_type_scalar(self->type TSRMLS_CC);
  RETURN_ZVAL(type, 1, 0);
}

PHP_METHOD(TypeSet, __toString)
{
  cassandra_type_set* self;
  smart_str string = {NULL, 0, 0};

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  php_cassandra_type_string((cassandra_type*)self, &string TSRMLS_CC);
  smart_str_0(&string);
  RETURN_STRING(string.c, 0);
}

PHP_METHOD(TypeSet, create)
{
  cassandra_type_set* self;
  cassandra_set* set;
  zval*** args;
  int argc = 0, i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*",
                            &args, &argc) == FAILURE) {
    return;
  }

  self = (cassandra_type_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_set_ce);
  set = (cassandra_set*) zend_object_store_get_object(return_value TSRMLS_CC);

  set->type = getThis();
  Z_ADDREF_P(set->type);

  if (argc > 0) {
    for (i = 0; i < argc; i++) {
      if (!php_cassandra_set_add(set, *args[i] TSRMLS_CC)) {
        efree(args);
        return;
      }
    }

    efree(args);
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_type_set_methods[] = {
  PHP_ME(TypeSet, name,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeSet, type,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeSet, __toString, arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeSet, create,     arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_type_set_handlers;

static void
php_cassandra_type_set_free(void *object TSRMLS_DC)
{
  cassandra_type_set* self = (cassandra_type_set*) object;

  if (self->value_type) zval_ptr_dtor(&self->value_type);

  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_type_set_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_type_set* self;

  self = (cassandra_type_set*) ecalloc(1, sizeof(cassandra_type_set));
  memset(self, 0, sizeof(cassandra_type_set));

  self->type = CASS_VALUE_TYPE_SET;
  self->value_type = NULL;

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  retval.handle   = zend_objects_store_put(self,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_type_set_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_type_set_handlers;

  return retval;
}

void cassandra_define_TypeSet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Set", cassandra_type_set_methods);
  cassandra_type_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_set_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_set_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_type_set_ce->create_object = php_cassandra_type_set_new;

  memcpy(&cassandra_type_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
