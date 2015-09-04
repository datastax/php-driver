#include "php_cassandra.h"
#include "util/types.h"
#include <ext/standard/php_smart_str.h>
#include "src/Cassandra/Collection.h"
#include "util/collections.h"

zend_class_entry *cassandra_type_collection_ce = NULL;

PHP_METHOD(TypeCollection, name)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  RETURN_STRING("list", 1);
}

PHP_METHOD(TypeCollection, type)
{
  cassandra_type_collection* self;
  zval* type;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_collection*) zend_object_store_get_object(getThis() TSRMLS_CC);
  type = php_cassandra_type_scalar(self->type TSRMLS_CC);
  RETURN_ZVAL(type, 1, 0);
}

PHP_METHOD(TypeCollection, __toString)
{
  cassandra_type_collection* self;
  const char* type;
  smart_str string = {NULL, 0, 0};

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_type_collection*) zend_object_store_get_object(getThis() TSRMLS_CC);
  type = php_cassandra_scalar_type_name(self->type TSRMLS_CC);

  smart_str_appendl(&string, "list<", 5);
  smart_str_appendl(&string, type, strlen(type));
  smart_str_appendl(&string, ">", 1);
  smart_str_0(&string);

  RETURN_STRING(string.c, 0);
}

PHP_METHOD(TypeCollection, create)
{
  cassandra_type_collection* self;
  cassandra_collection* collection;
  zval*** args;
  int argc = 0, i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*",
                            &args, &argc) == FAILURE) {
    return;
  }

  self = (cassandra_type_collection*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_collection_ce);
  collection = (cassandra_collection*) zend_object_store_get_object(return_value TSRMLS_CC);
  collection->type = self->type;

  if (argc > 0) {
    for (i = 0; i < argc; i++) {
      if (!php_cassandra_validate_object(*args[i], collection->type TSRMLS_CC)) {
        efree(args);
        return;
      }

      if (!php_cassandra_collection_add(collection, *args[i] TSRMLS_CC)) {
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

static zend_function_entry cassandra_type_collection_methods[] = {
  PHP_ME(TypeCollection, name,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, type,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, __toString, arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, create,     arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_type_collection_handlers;

static void
php_cassandra_type_collection_free(void *object TSRMLS_DC)
{
  cassandra_type_collection* collection = (cassandra_type_collection*) object;

  zend_object_std_dtor(&collection->zval TSRMLS_CC);

  efree(collection);
}

static zend_object_value
php_cassandra_type_collection_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_type_collection* collection;

  collection = (cassandra_type_collection*) ecalloc(1, sizeof(cassandra_type_collection));

  zend_object_std_init(&collection->zval, class_type TSRMLS_CC);
  object_properties_init(&collection->zval, class_type);

  collection->type = CASS_VALUE_TYPE_UNKNOWN;

  retval.handle   = zend_objects_store_put(collection,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_type_collection_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_type_collection_handlers;

  return retval;
}

void cassandra_define_TypeCollection(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Collection", cassandra_type_collection_methods);
  cassandra_type_collection_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_collection_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_collection_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_type_collection_ce->create_object = php_cassandra_type_collection_new;

  memcpy(&cassandra_type_collection_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
