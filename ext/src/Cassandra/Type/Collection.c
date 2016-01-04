#include "php_cassandra.h"
#include "util/types.h"
#include "src/Cassandra/Collection.h"
#include "util/collections.h"
#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *cassandra_type_collection_ce = NULL;

PHP_METHOD(TypeCollection, name)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  PHP5TO7_RETVAL_STRING("list");
}

PHP_METHOD(TypeCollection, type)
{
  php5to7_zval ztype;
  cassandra_type_collection *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE_COLLECTION(getThis());
  ztype = php_cassandra_type_scalar(self->type TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(ztype), 1, 0);
}

PHP_METHOD(TypeCollection, __toString)
{
  cassandra_type_collection *self;
  const char *type;
  smart_str string = PHP5TO7_SMART_STR_INIT;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE_COLLECTION(getThis());
  type = php_cassandra_scalar_type_name(self->type TSRMLS_CC);

  smart_str_appendl(&string, "list<", 5);
  smart_str_appendl(&string, type, strlen(type));
  smart_str_appendl(&string, ">", 1);
  smart_str_0(&string);

  PHP5TO7_RETVAL_STRING(PHP5TO7_SMART_STR_VAL(string));
  smart_str_free(&string);
}

PHP_METHOD(TypeCollection, create)
{
  cassandra_type_collection *self;
  cassandra_collection *collection;
  php5to7_zval_args args = NULL;
  int argc = 0, i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*",
                            &args, &argc) == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE_COLLECTION(getThis());

  object_init_ex(return_value, cassandra_collection_ce);
  collection = PHP_CASSANDRA_GET_COLLECTION(return_value);
  collection->type = self->type;

  if (argc > 0) {
    for (i = 0; i < argc; i++) {
      if (!php_cassandra_validate_object(PHP5TO7_ZVAL_ARG(args[i]), collection->type TSRMLS_CC)) {
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }

      if (!php_cassandra_collection_add(collection, PHP5TO7_ZVAL_ARG(args[i]) TSRMLS_CC)) {
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

static zend_function_entry cassandra_type_collection_methods[] = {
  PHP_ME(TypeCollection, name,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, type,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, __toString, arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, create,     arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_type_collection_handlers;

static void
php_cassandra_type_collection_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_type_collection *self =
      PHP5TO7_ZEND_OBJECT_GET(type_collection, object);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_type_collection_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_type_collection *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(type_collection, ce);

  self->type = CASS_VALUE_TYPE_UNKNOWN;

  PHP5TO7_ZEND_OBJECT_INIT(type_collection, self, ce);
}

void cassandra_define_TypeCollection(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Collection", cassandra_type_collection_methods);
  cassandra_type_collection_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_collection_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_collection_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_type_collection_ce->create_object = php_cassandra_type_collection_new;

  memcpy(&cassandra_type_collection_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
