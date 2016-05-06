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

#include "php_cassandra.h"
#include "util/result.h"
#include "util/ref.h"

zend_class_entry *cassandra_default_column_ce = NULL;

PHP_METHOD(DefaultColumn, name)
{
  cassandra_column *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_COLUMN(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->name), 1, 0);
}

PHP_METHOD(DefaultColumn, type)
{
  cassandra_column *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_COLUMN(getThis());

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->type), 1, 0);
}

PHP_METHOD(DefaultColumn, isReversed)
{
  cassandra_column *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_COLUMN(getThis());

  RETURN_BOOL(self->reversed);
}

PHP_METHOD(DefaultColumn, isStatic)
{
  cassandra_column *self;
  const CassValue  *value;
  const char       *str;
  size_t            str_len;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self  = PHP_CASSANDRA_GET_COLUMN(getThis());

  value = cass_column_meta_field_by_name(self->meta, "type");

  ASSERT_SUCCESS_BLOCK(cass_value_get_string(value, &str, &str_len),
    RETURN_FALSE;
  );

  if (strncmp("STATIC", str, str_len) == 0) {
    RETURN_TRUE;
  }

  RETURN_FALSE;
}

PHP_METHOD(DefaultColumn, isFrozen)
{
  cassandra_column *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_COLUMN(getThis());

  RETURN_BOOL(self->frozen);
}

PHP_METHOD(DefaultColumn, indexName)
{
  cassandra_column *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_COLUMN(getThis());

  php_cassandra_get_column_field(self->meta, "index_name", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultColumn, indexOptions)
{
  cassandra_column *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_COLUMN(getThis());

  php_cassandra_get_column_field(self->meta, "index_options", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_default_column_methods[] = {
  PHP_ME(DefaultColumn, name,         arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, type,         arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, isReversed,   arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, isStatic,     arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, isFrozen,     arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, indexName,    arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, indexOptions, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_default_column_handlers;

static HashTable *
php_cassandra_default_column_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_default_column_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_default_column_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_column *self = PHP5TO7_ZEND_OBJECT_GET(column, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->name);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->type);

  if (self->schema) {
    php_cassandra_del_ref(&self->schema);
    self->schema = NULL;
  }
  self->meta = NULL;

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_default_column_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_column *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(column, ce);

  self->reversed = 0;
  self->frozen   = 0;
  self->schema   = NULL;
  self->meta     = NULL;
  PHP5TO7_ZVAL_UNDEF(self->name);
  PHP5TO7_ZVAL_UNDEF(self->type);

  PHP5TO7_ZEND_OBJECT_INIT_EX(column, default_column, self, ce);
}

void cassandra_define_DefaultColumn(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\DefaultColumn", cassandra_default_column_methods);
  cassandra_default_column_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_default_column_ce TSRMLS_CC, 1, cassandra_column_ce);
  cassandra_default_column_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_default_column_ce->create_object = php_cassandra_default_column_new;

  memcpy(&cassandra_default_column_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_default_column_handlers.get_properties  = php_cassandra_default_column_properties;
  cassandra_default_column_handlers.compare_objects = php_cassandra_default_column_compare;
  cassandra_default_column_handlers.clone_obj = NULL;
}
