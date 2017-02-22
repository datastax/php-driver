/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/types.h"
#include "src/Collection.h"
#include "util/collections.h"

#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *php_driver_type_collection_ce = NULL;

PHP_METHOD(TypeCollection, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
    "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Type\\Collection type is not supported."
  );
  return;
}

PHP_METHOD(TypeCollection, name)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  PHP5TO7_RETVAL_STRING("list");
}

PHP_METHOD(TypeCollection, valueType)
{
  php_driver_type *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TYPE(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->data.collection.value_type), 1, 0);
}

PHP_METHOD(TypeCollection, __toString)
{
  php_driver_type *self;
  smart_str string = PHP5TO7_SMART_STR_INIT;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TYPE(getThis());

  php_driver_type_string(self, &string TSRMLS_CC);
  smart_str_0(&string);

  PHP5TO7_RETVAL_STRING(PHP5TO7_SMART_STR_VAL(string));
  smart_str_free(&string);
}

PHP_METHOD(TypeCollection, create)
{
  php_driver_type *self;
  php_driver_collection *collection;
  php5to7_zval_args args = NULL;
  int argc = 0, i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*",
                            &args, &argc) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TYPE(getThis());

  object_init_ex(return_value, php_driver_collection_ce);
  collection = PHP_DRIVER_GET_COLLECTION(return_value);

  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(collection->type), getThis());

  if (argc > 0) {
    for (i = 0; i < argc; i++) {
      if (!php_driver_validate_object(PHP5TO7_ZVAL_ARG(args[i]),
                                      PHP5TO7_ZVAL_MAYBE_P(self->data.collection.value_type) TSRMLS_CC)) {
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }

      php_driver_collection_add(collection, PHP5TO7_ZVAL_ARG(args[i]) TSRMLS_CC);
    }

    PHP5TO7_MAYBE_EFREE(args);
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_type_collection_methods[] = {
  PHP_ME(TypeCollection, __construct, arginfo_none,  ZEND_ACC_PRIVATE)
  PHP_ME(TypeCollection, name,        arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, valueType,   arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, __toString,  arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCollection, create,      arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_type_collection_handlers;

static HashTable *
php_driver_type_collection_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_type_collection_properties(zval *object TSRMLS_DC)
{
  php_driver_type *self  = PHP_DRIVER_GET_TYPE(object);
  HashTable      *props = zend_std_get_properties(object TSRMLS_CC);

  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "valueType", sizeof("valueType"),
                           PHP5TO7_ZVAL_MAYBE_P(self->data.collection.value_type), sizeof(zval));
  Z_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->data.collection.value_type));

  return props;
}

static int
php_driver_type_collection_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  php_driver_type* type1 = PHP_DRIVER_GET_TYPE(obj1);
  php_driver_type* type2 = PHP_DRIVER_GET_TYPE(obj2);

  return php_driver_type_compare(type1, type2 TSRMLS_CC);
}

static void
php_driver_type_collection_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_type *self = PHP5TO7_ZEND_OBJECT_GET(type, object);

  if (self->data_type) cass_data_type_free(self->data_type);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->data.collection.value_type);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_type_collection_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_type *self = PHP5TO7_ZEND_OBJECT_ECALLOC(type, ce);

  self->type = CASS_VALUE_TYPE_LIST;
  self->data_type = cass_data_type_new(self->type);
  PHP5TO7_ZVAL_UNDEF(self->data.collection.value_type);

  PHP5TO7_ZEND_OBJECT_INIT_EX(type, type_collection, self, ce);
}

void php_driver_define_TypeCollection(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Type\\Collection", php_driver_type_collection_methods);
  php_driver_type_collection_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_type_ce);
  memcpy(&php_driver_type_collection_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_type_collection_handlers.get_properties  = php_driver_type_collection_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_type_collection_handlers.get_gc          = php_driver_type_collection_gc;
#endif
  php_driver_type_collection_handlers.compare_objects = php_driver_type_collection_compare;
  php_driver_type_collection_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_type_collection_ce->create_object = php_driver_type_collection_new;
}
