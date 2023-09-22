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
#include "util/ref.h"

zend_class_entry *php_driver_default_schema_ce = NULL;

PHP_METHOD(DefaultSchema, keyspace)
{
  char *name;
  php5to7_size name_len;
  php_driver_schema *self;
  php_driver_keyspace *keyspace;
  const CassKeyspaceMeta *meta;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SCHEMA(getThis());
  meta = cass_schema_meta_keyspace_by_name_n((CassSchemaMeta *) self->schema->data, name, name_len);
  if (meta == NULL) {
    RETURN_FALSE;
  }

  object_init_ex(return_value, php_driver_default_keyspace_ce);
  keyspace = PHP_DRIVER_GET_KEYSPACE(return_value);
  keyspace->schema = php_driver_add_ref(self->schema);
  keyspace->meta   = meta;
}

PHP_METHOD(DefaultSchema, keyspaces)
{
  php_driver_schema *self;
  CassIterator     *iterator;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self     = PHP_DRIVER_GET_SCHEMA(getThis());
  iterator = cass_iterator_keyspaces_from_schema_meta((CassSchemaMeta *) self->schema->data);

  array_init(return_value);
  while (cass_iterator_next(iterator)) {
    const CassKeyspaceMeta  *meta;
    const CassValue         *value;
    const char              *keyspace_name;
    size_t                   keyspace_name_len;
    php5to7_zval             zkeyspace;
    php_driver_keyspace      *keyspace;

    meta = cass_iterator_get_keyspace_meta(iterator);
    value = cass_keyspace_meta_field_by_name(meta, "keyspace_name");

    ASSERT_SUCCESS_BLOCK(cass_value_get_string(value, &keyspace_name, &keyspace_name_len),
      zval_ptr_dtor(PHP5TO7_ZVAL_MAYBE_ADDR_OF(return_value));
      return;
    );

    PHP5TO7_ZVAL_MAYBE_MAKE(zkeyspace);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(zkeyspace), php_driver_default_keyspace_ce);
    keyspace = PHP_DRIVER_GET_KEYSPACE(PHP5TO7_ZVAL_MAYBE_P(zkeyspace));
    keyspace->schema = php_driver_add_ref(self->schema);
    keyspace->meta   = meta;
    PHP5TO7_ADD_ASSOC_ZVAL_EX(return_value,
                              keyspace_name, keyspace_name_len + 1,
                              PHP5TO7_ZVAL_MAYBE_P(zkeyspace));
  }

  cass_iterator_free(iterator);
}

PHP_METHOD(DefaultSchema, version)
{
  php_driver_schema *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_SCHEMA(getThis());
  RETURN_LONG(cass_schema_meta_snapshot_version((CassSchemaMeta *) self->schema->data));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_default_schema_methods[] = {
  PHP_ME(DefaultSchema, keyspace, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSchema, keyspaces, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSchema, version, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_default_schema_handlers;

static HashTable *
php_driver_default_schema_properties(php7to8_object *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_default_schema_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_default_schema_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_schema *self = PHP5TO7_ZEND_OBJECT_GET(schema, object);

  if (self->schema) {
    php_driver_del_ref(&self->schema);
    self->schema = NULL;
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_default_schema_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_schema *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(schema, ce);

  self->schema = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(schema, default_schema, self, ce);
}

void php_driver_define_DefaultSchema(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\DefaultSchema", php_driver_default_schema_methods);
  php_driver_default_schema_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_default_schema_ce TSRMLS_CC, 1, php_driver_schema_ce);
  php_driver_default_schema_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_default_schema_ce->create_object = php_driver_default_schema_new;

  memcpy(&php_driver_default_schema_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_default_schema_handlers.get_properties  = php_driver_default_schema_properties;
  PHP7TO8_COMPARE(php_driver_default_schema_handlers, php_driver_default_schema_compare);
  php_driver_default_schema_handlers.clone_obj = NULL;
}
