/**
 * Copyright 2015-2017 DataStax, Inc.
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

zend_class_entry *php_driver_prepared_statement_ce = NULL;

PHP_METHOD(PreparedStatement, __construct)
{
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_prepared_statement_methods[] = {
  PHP_ME(PreparedStatement, __construct, arginfo_none, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
  PHP_FE_END
};

static zend_object_handlers php_driver_prepared_statement_handlers;

static HashTable *
php_driver_prepared_statement_properties(php7to8_object *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_prepared_statement_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_prepared_statement_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_statement *self = PHP5TO7_ZEND_OBJECT_GET(statement, object);

  if (self->data.prepared.prepared)
    cass_prepared_free(self->data.prepared.prepared);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_prepared_statement_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_statement *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(statement, ce);

  self->type = PHP_DRIVER_PREPARED_STATEMENT;
  self->data.prepared.prepared = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(statement, prepared_statement, self, ce);
}

void php_driver_define_PreparedStatement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\PreparedStatement", php_driver_prepared_statement_methods);
  php_driver_prepared_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_prepared_statement_ce TSRMLS_CC, 1, php_driver_statement_ce);
  php_driver_prepared_statement_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_prepared_statement_ce->create_object = php_driver_prepared_statement_new;

  memcpy(&php_driver_prepared_statement_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_prepared_statement_handlers.get_properties  = php_driver_prepared_statement_properties;
  PHP7TO8_COMPARE(php_driver_prepared_statement_handlers, php_driver_prepared_statement_compare);
  php_driver_prepared_statement_handlers.clone_obj = NULL;
}
