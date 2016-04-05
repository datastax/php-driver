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
#include "util/types.h"

zend_class_entry *cassandra_default_keyspace_ce = NULL;

PHP_METHOD(DefaultKeyspace, name)
{
  cassandra_keyspace *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_KEYSPACE(getThis());

  php_cassandra_get_keyspace_field(self->meta, "keyspace_name", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultKeyspace, replicationClassName)
{
  cassandra_keyspace *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_KEYSPACE(getThis());

  php_cassandra_get_keyspace_field(self->meta, "strategy_class", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultKeyspace, replicationOptions)
{
  cassandra_keyspace *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_KEYSPACE(getThis());

  php_cassandra_get_keyspace_field(self->meta, "strategy_options", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultKeyspace, hasDurableWrites)
{
  cassandra_keyspace *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_KEYSPACE(getThis());

  php_cassandra_get_keyspace_field(self->meta, "durable_writes", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultKeyspace, table)
{
  char *name;
  php5to7_size name_len;
  cassandra_keyspace *self;
  cassandra_table *table;
  cassandra_table_meta *meta;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_KEYSPACE(getThis());
  meta = cass_keyspace_meta_table_by_name(self->meta, name);

  if (meta == NULL) {
    return;
  }

  object_init_ex(return_value, cassandra_default_table_ce);
  table = PHP_CASSANDRA_GET_TABLE(return_value);
  table->schema = php_cassandra_add_ref(self->schema);
  table->meta   = meta;
}

PHP_METHOD(DefaultKeyspace, tables)
{
  cassandra_keyspace *self;
  CassIterator       *iterator;
  if (zend_parse_parameters_none() == FAILURE)
    return;

  self     = PHP_CASSANDRA_GET_KEYSPACE(getThis());
  iterator = cass_iterator_tables_from_keyspace_meta(self->meta);

  array_init(return_value);
  while (cass_iterator_next(iterator)) {
    cassandra_table_meta *meta;
    const CassValue      *value;
    const char           *table_name;
    size_t                table_name_len;
    php5to7_zval          ztable;
    cassandra_table      *table;

    meta = cass_iterator_get_table_meta(iterator);
    value = cass_table_meta_field_by_name(meta, "columnfamily_name");

    ASSERT_SUCCESS_BLOCK(cass_value_get_string(value, &table_name, &table_name_len),
      zval_ptr_dtor(PHP5TO7_ZVAL_MAYBE_ADDR_OF(return_value));
      return;
    );

    PHP5TO7_ZVAL_MAYBE_MAKE(ztable);
    object_init_ex(PHP5TO7_ZVAL_MAYBE_P(ztable), cassandra_default_table_ce);
    table = PHP_CASSANDRA_GET_TABLE(PHP5TO7_ZVAL_MAYBE_P(ztable));
    table->schema = php_cassandra_add_ref(self->schema);
    table->meta   = meta;
    PHP5TO7_ADD_ASSOC_ZVAL_EX(return_value,
                              table_name, table_name_len + 1,
                              PHP5TO7_ZVAL_MAYBE_P(ztable));
  }

  cass_iterator_free(iterator);
}

PHP_METHOD(DefaultKeyspace, userType)
{
  char *name;
  php5to7_size name_len;
  cassandra_keyspace *self;
  php5to7_zval ztype;
  const CassDataType *user_type;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_KEYSPACE(getThis());
  user_type = cass_keyspace_meta_user_type_by_name_n(self->meta, name, name_len);

  if (user_type == NULL) {
    return;
  }

  ztype = php_cassandra_type_from_data_type(user_type TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(ztype), 0, 1);
}

PHP_METHOD(DefaultKeyspace, userTypes)
{
  cassandra_keyspace *self;
  CassIterator       *iterator;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self     = PHP_CASSANDRA_GET_KEYSPACE(getThis());
  iterator = cass_iterator_user_types_from_keyspace_meta(self->meta);

  array_init(return_value);
  while (cass_iterator_next(iterator)) {
    const CassDataType *user_type;
    php5to7_zval ztype;
    const char *type_name;
    size_t type_name_len;

    user_type = cass_iterator_get_user_type(iterator);
    ztype = php_cassandra_type_from_data_type(user_type TSRMLS_CC);

    cass_data_type_type_name(user_type, &type_name, &type_name_len);
    PHP5TO7_ADD_ASSOC_ZVAL_EX(return_value,
                              type_name, type_name_len + 1,
                              PHP5TO7_ZVAL_MAYBE_P(ztype));
  }

  cass_iterator_free(iterator);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_default_keyspace_methods[] = {
  PHP_ME(DefaultKeyspace, name, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultKeyspace, replicationClassName, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultKeyspace, replicationOptions, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultKeyspace, hasDurableWrites, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultKeyspace, table, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultKeyspace, tables, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultKeyspace, userType, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultKeyspace, userTypes, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_default_keyspace_handlers;

static HashTable *
php_cassandra_default_keyspace_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_default_keyspace_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_default_keyspace_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_keyspace *self = PHP5TO7_ZEND_OBJECT_GET(keyspace, object);

  if (self->schema) {
    php_cassandra_del_ref(&self->schema);
    self->schema = NULL;
  }
  self->meta = NULL;

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_default_keyspace_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_keyspace *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(keyspace, ce);

  self->meta   = NULL;
  self->schema = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(keyspace, default_keyspace, self, ce);
}

void cassandra_define_DefaultKeyspace(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\DefaultKeyspace", cassandra_default_keyspace_methods);
  cassandra_default_keyspace_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_default_keyspace_ce TSRMLS_CC, 1, cassandra_keyspace_ce);
  cassandra_default_keyspace_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_default_keyspace_ce->create_object = php_cassandra_default_keyspace_new;

  memcpy(&cassandra_default_keyspace_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_default_keyspace_handlers.get_properties  = php_cassandra_default_keyspace_properties;
  cassandra_default_keyspace_handlers.compare_objects = php_cassandra_default_keyspace_compare;
  cassandra_default_keyspace_handlers.clone_obj = NULL;
}
