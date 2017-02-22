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

zend_class_entry *php_driver_type_custom_ce = NULL;

PHP_METHOD(TypeCustom, __construct)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
    "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Type\\Custom type is not supported."
  );
  return;
}

PHP_METHOD(TypeCustom, name)
{
  php_driver_type *custom;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  custom = PHP_DRIVER_GET_TYPE(getThis());

  PHP5TO7_RETVAL_STRING(custom->data.custom.class_name);
}

PHP_METHOD(TypeCustom, __toString)
{
  php_driver_type *custom;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  custom = PHP_DRIVER_GET_TYPE(getThis());

  PHP5TO7_RETVAL_STRING(custom->data.custom.class_name);
}

PHP_METHOD(TypeCustom, create)
{
  zend_throw_exception_ex(php_driver_logic_exception_ce, 0 TSRMLS_CC,
    "Instantiation of a " PHP_DRIVER_NAMESPACE "\\Type\\Custom instance is not supported."
  );
  return;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_type_custom_methods[] = {
  PHP_ME(TypeCustom, __construct, arginfo_none,  ZEND_ACC_PRIVATE)
  PHP_ME(TypeCustom, name,        arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCustom, __toString,  arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeCustom, create,      arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_type_custom_handlers;

static HashTable *
php_driver_type_custom_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_type_custom_properties(zval *object TSRMLS_DC)
{
  php5to7_zval name;

  php_driver_type *self  = PHP_DRIVER_GET_TYPE(object);
  HashTable      *props = zend_std_get_properties(object TSRMLS_CC);

  PHP5TO7_ZVAL_MAYBE_MAKE(name);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(name), self->data.custom.class_name);

  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "name", sizeof("name"),
                           PHP5TO7_ZVAL_MAYBE_P(name), sizeof(zval));
  return props;
}

static int
php_driver_type_custom_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  php_driver_type* type1 = PHP_DRIVER_GET_TYPE(obj1);
  php_driver_type* type2 = PHP_DRIVER_GET_TYPE(obj2);

  return php_driver_type_compare(type1, type2 TSRMLS_CC);
}

static void
php_driver_type_custom_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_type *self = PHP5TO7_ZEND_OBJECT_GET(type, object);

  if (self->data_type) cass_data_type_free(self->data_type);
  if (self->data.custom.class_name) {
    efree(self->data.custom.class_name);
    self->data.custom.class_name = NULL;
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_type_custom_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_type *self = PHP5TO7_ZEND_OBJECT_ECALLOC(type, ce);

  self->type = CASS_VALUE_TYPE_CUSTOM;
  self->data_type = cass_data_type_new(self->type);
  self->data.custom.class_name = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(type, type_custom, self, ce);
}

void php_driver_define_TypeCustom(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Type\\Custom", php_driver_type_custom_methods);
  php_driver_type_custom_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_type_ce);
  memcpy(&php_driver_type_custom_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_type_custom_handlers.get_properties  = php_driver_type_custom_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_type_custom_handlers.get_gc          = php_driver_type_custom_gc;
#endif
  php_driver_type_custom_handlers.compare_objects = php_driver_type_custom_compare;
  php_driver_type_custom_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_type_custom_ce->create_object = php_driver_type_custom_new;
}
