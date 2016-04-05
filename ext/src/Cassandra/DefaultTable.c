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

zend_class_entry *cassandra_default_table_ce = NULL;

PHP_METHOD(DefaultTable, name)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "columnfamily_name", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, comment)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "comment", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, readRepairChance)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "read_repair_chance", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, localReadRepairChance)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "local_read_repair_chance", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, gcGraceSeconds)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "gc_grace_seconds", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, caching)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "caching", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, bloomFilterFPChance)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "bloom_filter_fp_chance", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, memtableFlushPeriodMs)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "memtable_flush_period_in_ms", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, defaultTTL)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "default_time_to_live", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, speculativeRetry)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "speculative_retry", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, indexInterval)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "index_interval", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, compactionStrategyClassName)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "compaction_strategy_class", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, compactionStrategyOptions)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "compaction_strategy_options", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, compressionParameters)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "compression_parameters", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

static php5to7_zval
php_cassandra_create_column(cassandra_ref *schema,
                            cassandra_column_meta *meta TSRMLS_DC)
{
  php5to7_zval result;
  cassandra_column *column;
  const CassValue *value;

  const char *validator;
  size_t      validator_length;

  PHP5TO7_ZVAL_UNDEF(result);


  value = cass_column_meta_field_by_name(meta, "validator");

  ASSERT_SUCCESS_VALUE(cass_value_get_string(value, &validator,
                                             &validator_length), result);

  PHP5TO7_ZVAL_MAYBE_MAKE(result);
  object_init_ex(PHP5TO7_ZVAL_MAYBE_P(result), cassandra_default_column_ce);

  column = PHP_CASSANDRA_GET_COLUMN(PHP5TO7_ZVAL_MAYBE_P(result));

  ASSERT_SUCCESS_BLOCK(php_cassandra_get_column_field(meta, "column_name",
                                                      &column->name TSRMLS_CC),
    zval_ptr_dtor(&result);
    PHP5TO7_ZVAL_UNDEF(result);
    return result;
  );

  if (php_cassandra_parse_column_type(validator, validator_length,
                                      &column->reversed, &column->frozen,
                                      &column->type TSRMLS_CC) == FAILURE) {
    zval_ptr_dtor(&result);
    PHP5TO7_ZVAL_UNDEF(result);
    return result;
  }

  column->meta   = meta;
  column->schema = php_cassandra_add_ref(schema);

  return result;
}

PHP_METHOD(DefaultTable, column)
{
  cassandra_table *self;
  char *name;
  php5to7_size name_len;
  php5to7_zval column;
  cassandra_column_meta *meta;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TABLE(getThis());
  meta = cass_table_meta_column_by_name(self->meta, name);

  if (meta == NULL) {
    return;
  }

  column = php_cassandra_create_column(self->schema, meta TSRMLS_CC);

  if (PHP5TO7_ZVAL_IS_UNDEF(column)) {
    return;
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(column), 0, 1);
}

PHP_METHOD(DefaultTable, columns)
{
  cassandra_table *self;
  CassIterator    *iterator;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self     = PHP_CASSANDRA_GET_TABLE(getThis());
  iterator = cass_iterator_columns_from_table_meta(self->meta);

  array_init(return_value);
  while (cass_iterator_next(iterator)) {
    cassandra_column_meta *meta;
    php5to7_zval zcolumn;
    cassandra_column *column;

    meta    = cass_iterator_get_column_meta(iterator);
    zcolumn = php_cassandra_create_column(self->schema, meta TSRMLS_CC);

    if (PHP5TO7_ZVAL_IS_UNDEF(zcolumn)) {
      zval_ptr_dtor(PHP5TO7_ZVAL_MAYBE_ADDR_OF(return_value));
      cass_iterator_free(iterator);
      return;
    } else {
      column = PHP_CASSANDRA_GET_COLUMN(PHP5TO7_ZVAL_MAYBE_P(zcolumn));

      if (PHP5TO7_Z_TYPE_MAYBE_P(column->name) == IS_STRING) {
        PHP5TO7_ADD_ASSOC_ZVAL_EX(return_value,
                                  PHP5TO7_Z_STRVAL_MAYBE_P(column->name),
                                  PHP5TO7_Z_STRLEN_MAYBE_P(column->name) + 1,
                                  PHP5TO7_ZVAL_MAYBE_P(zcolumn));
      } else {
        add_next_index_zval(return_value, PHP5TO7_ZVAL_MAYBE_P(zcolumn));
      }
    }
  }

  cass_iterator_free(iterator);
}

PHP_METHOD(DefaultTable, populateIOCacheOnFlush)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "populate_io_cache_on_flush", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, replicateOnWrite)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "replicate_on_write", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, maxIndexInterval)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "max_index_interval", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

PHP_METHOD(DefaultTable, minIndexInterval)
{
  cassandra_table *self;
  php5to7_zval value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_TABLE(getThis());

  php_cassandra_get_table_field(self->meta, "min_index_interval", &value TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 0, 1);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_default_table_methods[] = {
  PHP_ME(DefaultTable, name, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, comment, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, readRepairChance, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, localReadRepairChance, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, gcGraceSeconds, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, caching, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, bloomFilterFPChance, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, memtableFlushPeriodMs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, defaultTTL, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, speculativeRetry, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, indexInterval, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, compactionStrategyClassName, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, compactionStrategyOptions, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, compressionParameters, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, populateIOCacheOnFlush, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, replicateOnWrite, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, maxIndexInterval, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, minIndexInterval, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, column, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, columns, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_default_table_handlers;

static HashTable *
php_cassandra_default_table_properties(zval *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_default_table_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_default_table_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_table *self = PHP5TO7_ZEND_OBJECT_GET(table, object);

  if (self->schema) {
    php_cassandra_del_ref(&self->schema);
    self->schema = NULL;
  }
  self->meta = NULL;

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_default_table_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_table *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(table, ce);

  self->meta   = NULL;
  self->schema = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(table, default_table, self, ce);
}

void cassandra_define_DefaultTable(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\DefaultTable", cassandra_default_table_methods);
  cassandra_default_table_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_default_table_ce TSRMLS_CC, 1, cassandra_table_ce);
  cassandra_default_table_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_default_table_ce->create_object = php_cassandra_default_table_new;

  memcpy(&cassandra_default_table_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_default_table_handlers.get_properties  = php_cassandra_default_table_properties;
  cassandra_default_table_handlers.compare_objects = php_cassandra_default_table_compare;
  cassandra_default_table_handlers.clone_obj = NULL;
}
