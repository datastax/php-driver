#include "php_cassandra.h"
#include "util/types.h"
#include "src/Cassandra/Map.h"

zend_class_entry *cassandra_type_map_ce = NULL;

PHP_METHOD(TypeMap, name)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  RETURN_STRING("map", 1);
}

PHP_METHOD(TypeMap, keyType)
{
  cassandra_type_map* self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->key_type, 1, 0);
}

PHP_METHOD(TypeMap, valueType)
{
  cassandra_type_map* self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->value_type, 1, 0);
}

PHP_METHOD(TypeMap, __toString)
{
  cassandra_type_map* self;
  smart_str string = {NULL, 0, 0};

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  php_cassandra_type_string((cassandra_type*)self, &string TSRMLS_CC);
  smart_str_0(&string);
  RETURN_STRING(string.c, 0);
}

PHP_METHOD(TypeMap, create)
{
  cassandra_type_map* self;
  cassandra_map* map;
  zval*** args;
  int argc = 0, i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*",
                            &args, &argc) == FAILURE) {
    return;
  }

  if (argc % 2 == 1) {
    efree(args);
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Not enough values, maps can only be created " \
                            "from an even number of values, where each odd " \
                            "value is a key and each even value is a value, " \
                            "e.g create(key, value, key, value, key, value)");
    return;
  }

  self = (cassandra_type_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_map_ce);
  map = (cassandra_map*) zend_object_store_get_object(return_value TSRMLS_CC);

  map->type = getThis();
  Z_ADDREF_P(map->type);

  if (argc > 0) {
    for (i = 0; i < argc; i += 2) {
      if (!php_cassandra_map_set(map, *args[i], *args[i + 1] TSRMLS_CC)) {
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

static zend_function_entry cassandra_type_map_methods[] = {
  PHP_ME(TypeMap, name,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeMap, keyType,    arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeMap, valueType,  arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeMap, __toString, arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeMap, create,     arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_type_map_handlers;

static void
php_cassandra_type_map_free(void *object TSRMLS_DC)
{
  cassandra_type_map* self = (cassandra_type_map*) object;

  if (self->key_type) zval_ptr_dtor(&self->key_type);
  if (self->value_type) zval_ptr_dtor(&self->value_type);

  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_type_map_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_type_map* self;

  self = (cassandra_type_map*) ecalloc(1, sizeof(cassandra_type_map));
  memset(self, 0, sizeof(cassandra_type_map));

  self->type = CASS_VALUE_TYPE_MAP;
  self->key_type = NULL;
  self->value_type = NULL;

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  retval.handle   = zend_objects_store_put(self,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_type_map_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_type_map_handlers;

  return retval;
}

void cassandra_define_TypeMap(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Map", cassandra_type_map_methods);
  cassandra_type_map_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_map_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_map_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_type_map_ce->create_object = php_cassandra_type_map_new;

  memcpy(&cassandra_type_map_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
