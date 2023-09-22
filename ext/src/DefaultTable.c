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
#include "util/result.h"
#include "util/ref.h"
#include "util/types.h"

#include "DefaultColumn.h"
#include "DefaultMaterializedView.h"
#include "DefaultIndex.h"
#include "Table.h"


zend_class_entry *php_driver_default_table_ce = NULL;

static void
populate_partition_key(php_driver_table *table, zval *result TSRMLS_DC)
{
  size_t i, count = cass_table_meta_partition_key_count(table->meta);
  for (i = 0; i < count; ++i) {
    const CassColumnMeta *column =
      cass_table_meta_partition_key(table->meta, i);
    if (column) {
      php5to7_zval zcolumn = php_driver_create_column(table->schema, column TSRMLS_CC);
      if (!PHP5TO7_ZVAL_IS_UNDEF(zcolumn)) {
        add_next_index_zval(result, PHP5TO7_ZVAL_MAYBE_P(zcolumn));
      }
    }
  }
}

static void
populate_clustering_key(php_driver_table *table, zval *result TSRMLS_DC)
{
  size_t i, count = cass_table_meta_clustering_key_count(table->meta);
  for (i = 0; i < count; ++i) {
    const CassColumnMeta *column =
        cass_table_meta_clustering_key(table->meta, i);
    if (column) {
      php5to7_zval zcolumn = php_driver_create_column(table->schema, column TSRMLS_CC);
      if (!PHP5TO7_ZVAL_IS_UNDEF(zcolumn)) {
        add_next_index_zval(result, PHP5TO7_ZVAL_MAYBE_P(zcolumn));
      }
    }
  }
}

php5to7_zval
php_driver_create_table(php_driver_ref* schema,
                           const CassTableMeta *meta TSRMLS_DC)
{
  php5to7_zval result;
  php_driver_table *table;
  const char *name;
  size_t name_length;

  PHP5TO7_ZVAL_UNDEF(result);

  PHP5TO7_ZVAL_MAYBE_MAKE(result);
  object_init_ex(PHP5TO7_ZVAL_MAYBE_P(result), php_driver_default_table_ce);

  table = PHP_DRIVER_GET_TABLE(PHP5TO7_ZVAL_MAYBE_P(result));
  table->schema = php_driver_add_ref(schema);
  table->meta   = meta;

  cass_table_meta_name(meta, &name, &name_length);
  PHP5TO7_ZVAL_MAYBE_MAKE(table->name);
  PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(table->name), name, name_length);

  return result;
}

void
php_driver_default_table_build_options(php_driver_table *table TSRMLS_DC) {
  CassIterator *iterator =
      cass_iterator_fields_from_table_meta(table->meta);
  table->options = php_driver_table_build_options(iterator TSRMLS_CC);
  cass_iterator_free(iterator);
}

void
php_driver_table_get_option(php_driver_table *table,
                               const char *name,
                               zval *result TSRMLS_DC) {
  zval *zvalue;
  if (PHP5TO7_ZVAL_IS_UNDEF(table->options)) {
    php_driver_default_table_build_options(table TSRMLS_CC);
  }

  if (!PHP5TO7_ZEND_HASH_FIND(PHP5TO7_Z_ARRVAL_MAYBE_P(table->options),
                         name, strlen(name) + 1,
                         zvalue)) {
    ZVAL_FALSE(result);
    return;
  }

  PHP5TO7_ZVAL_COPY(result, zvalue);
}

PHP_METHOD(DefaultTable, name)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->name), 1, 0);
}

PHP_METHOD(DefaultTable, option)
{
  char *name;
  php5to7_size name_len;
  php_driver_table *self;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TABLE(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->options)) {
    php_driver_default_table_build_options(self TSRMLS_CC);
  }

  if (PHP5TO7_ZEND_HASH_FIND(PHP5TO7_Z_ARRVAL_MAYBE_P(self->options),
                         name, name_len + 1,
                         result)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(result), 1, 0);
  }
  RETURN_FALSE;
}

PHP_METHOD(DefaultTable, options)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->options)) {
    php_driver_default_table_build_options(self TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->options), 1, 0);
}

PHP_METHOD(DefaultTable, comment)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "comment", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, readRepairChance)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "read_repair_chance", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, localReadRepairChance)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "local_read_repair_chance", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, gcGraceSeconds)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "gc_grace_seconds", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, caching)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "caching", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, bloomFilterFPChance)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "bloom_filter_fp_chance", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, memtableFlushPeriodMs)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "memtable_flush_period_in_ms", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, defaultTTL)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "default_time_to_live", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, speculativeRetry)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "speculative_retry", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, indexInterval)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "index_interval", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, compactionStrategyClassName)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "compaction_strategy_class", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, compactionStrategyOptions)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "compaction_strategy_options", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, compressionParameters)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "compression_parameters", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, populateIOCacheOnFlush)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "populate_io_cache_on_flush", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, replicateOnWrite)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "replicate_on_write", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, maxIndexInterval)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "max_index_interval", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultTable, minIndexInterval)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());

  php_driver_table_get_option(self, "min_index_interval", return_value TSRMLS_CC);
}


PHP_METHOD(DefaultTable, column)
{
  php_driver_table *self;
  char *name;
  php5to7_size name_len;
  php5to7_zval column;
  const CassColumnMeta *meta;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TABLE(getThis());
  meta = cass_table_meta_column_by_name(self->meta, name);
  if (meta == NULL) {
    RETURN_FALSE;
  }

  column = php_driver_create_column(self->schema, meta TSRMLS_CC);

  if (PHP5TO7_ZVAL_IS_UNDEF(column)) {
    return;
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(column), 0, 1);
}

PHP_METHOD(DefaultTable, columns)
{
  php_driver_table *self;
  CassIterator    *iterator;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self     = PHP_DRIVER_GET_TABLE(getThis());
  iterator = cass_iterator_columns_from_table_meta(self->meta);

  array_init(return_value);
  while (cass_iterator_next(iterator)) {
    const CassColumnMeta *meta;
    php5to7_zval zcolumn;
    php_driver_column *column;

    meta    = cass_iterator_get_column_meta(iterator);
    zcolumn = php_driver_create_column(self->schema, meta TSRMLS_CC);

    if (!PHP5TO7_ZVAL_IS_UNDEF(zcolumn)) {
      column = PHP_DRIVER_GET_COLUMN(PHP5TO7_ZVAL_MAYBE_P(zcolumn));

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

PHP_METHOD(DefaultTable, partitionKey)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->partition_key)) {
    PHP5TO7_ZVAL_MAYBE_MAKE(self->partition_key);
    array_init(PHP5TO7_ZVAL_MAYBE_P(self->partition_key));
    populate_partition_key(self, PHP5TO7_ZVAL_MAYBE_P(self->partition_key) TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->partition_key), 1, 0);
}

PHP_METHOD(DefaultTable, primaryKey)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->primary_key)) {
    PHP5TO7_ZVAL_MAYBE_MAKE(self->primary_key);
    array_init(PHP5TO7_ZVAL_MAYBE_P(self->primary_key));
    populate_partition_key(self, PHP5TO7_ZVAL_MAYBE_P(self->primary_key) TSRMLS_CC);
    populate_clustering_key(self, PHP5TO7_ZVAL_MAYBE_P(self->primary_key) TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->primary_key), 1, 0);
}

PHP_METHOD(DefaultTable, clusteringKey)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->clustering_key)) {
    PHP5TO7_ZVAL_MAYBE_MAKE(self->clustering_key);
    array_init(PHP5TO7_ZVAL_MAYBE_P(self->clustering_key));
    populate_clustering_key(self, PHP5TO7_ZVAL_MAYBE_P(self->clustering_key) TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->clustering_key), 1, 0);
}

PHP_METHOD(DefaultTable, clusteringOrder)
{
  php_driver_table *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_TABLE(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->clustering_order)) {
    size_t i, count = cass_table_meta_clustering_key_count(self->meta);
    PHP5TO7_ZVAL_MAYBE_MAKE(self->clustering_order);
    array_init(PHP5TO7_ZVAL_MAYBE_P(self->clustering_order));
    for (i = 0; i < count; ++i) {
      CassClusteringOrder order =
          cass_table_meta_clustering_key_order(self->meta, i);
      switch (order) {
        case CASS_CLUSTERING_ORDER_ASC:
          PHP5TO7_ADD_NEXT_INDEX_STRING(PHP5TO7_ZVAL_MAYBE_P(self->clustering_order), "asc");
          break;
        case CASS_CLUSTERING_ORDER_DESC:
          PHP5TO7_ADD_NEXT_INDEX_STRING(PHP5TO7_ZVAL_MAYBE_P(self->clustering_order), "desc");
          break;
        case CASS_CLUSTERING_ORDER_NONE:
          PHP5TO7_ADD_NEXT_INDEX_STRING(PHP5TO7_ZVAL_MAYBE_P(self->clustering_order), "none");
          break;
      }
    }
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->clustering_order), 1, 0);
}

PHP_METHOD(DefaultTable, index)
{
  php_driver_table *self;
  char *name;
  php5to7_size name_len;
  php5to7_zval index;
  const CassIndexMeta *meta;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TABLE(getThis());
  meta = cass_table_meta_index_by_name(self->meta, name);
  if (meta == NULL) {
    RETURN_FALSE;
  }

  index = php_driver_create_index(self->schema, meta TSRMLS_CC);
  if (PHP5TO7_ZVAL_IS_UNDEF(index)) {
    return;
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(index), 0, 1);
}

PHP_METHOD(DefaultTable, indexes)
{
  php_driver_table *self;
  CassIterator *iterator;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self     = PHP_DRIVER_GET_TABLE(getThis());
  iterator = cass_iterator_indexes_from_table_meta(self->meta);

  array_init(return_value);
  while (cass_iterator_next(iterator)) {
    const CassIndexMeta *meta;
    php5to7_zval zindex;

    meta   = cass_iterator_get_index_meta(iterator);
    zindex = php_driver_create_index(self->schema, meta TSRMLS_CC);

    if (!PHP5TO7_ZVAL_IS_UNDEF(zindex)) {
      php_driver_index *index = PHP_DRIVER_GET_INDEX(PHP5TO7_ZVAL_MAYBE_P(zindex));

      if (PHP5TO7_Z_TYPE_MAYBE_P(index->name) == IS_STRING) {
        PHP5TO7_ADD_ASSOC_ZVAL_EX(return_value,
                                  PHP5TO7_Z_STRVAL_MAYBE_P(index->name),
                                  PHP5TO7_Z_STRLEN_MAYBE_P(index->name) + 1,
                                  PHP5TO7_ZVAL_MAYBE_P(zindex));
      } else {
        add_next_index_zval(return_value, PHP5TO7_ZVAL_MAYBE_P(zindex));
      }
    }
  }

  cass_iterator_free(iterator);
}

PHP_METHOD(DefaultTable, materializedView)
{
  php_driver_table *self;
  char *name;
  php5to7_size name_len;
  php5to7_zval zview;
  const CassMaterializedViewMeta *meta;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_TABLE(getThis());
  meta = cass_table_meta_materialized_view_by_name_n(self->meta,
                                                     name, name_len);
  if (meta == NULL) {
    RETURN_FALSE;
  }

  zview = php_driver_create_materialized_view(self->schema, meta TSRMLS_CC);
  if (PHP5TO7_ZVAL_IS_UNDEF(zview)) {
    return;
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(zview), 0, 1);
}

PHP_METHOD(DefaultTable, materializedViews)
{
  php_driver_table *self;
  CassIterator *iterator;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self     = PHP_DRIVER_GET_TABLE(getThis());
  iterator = cass_iterator_materialized_views_from_table_meta(self->meta);

  array_init(return_value);
  while (cass_iterator_next(iterator)) {
    const CassMaterializedViewMeta *meta;
    php5to7_zval zview;
    php_driver_materialized_view *view;

    meta  = cass_iterator_get_materialized_view_meta(iterator);
    zview = php_driver_create_materialized_view(self->schema, meta TSRMLS_CC);

    if (!PHP5TO7_ZVAL_IS_UNDEF(zview)) {
      view = PHP_DRIVER_GET_MATERIALIZED_VIEW(PHP5TO7_ZVAL_MAYBE_P(zview));

      if (PHP5TO7_Z_TYPE_MAYBE_P(view->name) == IS_STRING) {
        PHP5TO7_ADD_ASSOC_ZVAL_EX(return_value,
                                  PHP5TO7_Z_STRVAL_MAYBE_P(view->name),
                                  PHP5TO7_Z_STRLEN_MAYBE_P(view->name) + 1,
                                  PHP5TO7_ZVAL_MAYBE_P(zview));
      } else {
        add_next_index_zval(return_value, PHP5TO7_ZVAL_MAYBE_P(zview));
      }
    }
  }

  cass_iterator_free(iterator);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_default_table_methods[] = {
  PHP_ME(DefaultTable, name, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, option, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, options, arginfo_none, ZEND_ACC_PUBLIC)
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
  PHP_ME(DefaultTable, partitionKey, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, primaryKey, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, clusteringKey, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, clusteringOrder, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, index, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, indexes, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, materializedView, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultTable, materializedViews, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_default_table_handlers;

static HashTable *
php_driver_type_default_table_gc(php7to8_object *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_default_table_properties(php7to8_object *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_default_table_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_default_table_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_table *self = PHP5TO7_ZEND_OBJECT_GET(table, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->name);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->options);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->partition_key);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->primary_key);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->clustering_key);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->clustering_order);

  if (self->schema) {
    php_driver_del_ref(&self->schema);
    self->schema = NULL;
  }
  self->meta = NULL;

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_default_table_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_table *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(table, ce);

  PHP5TO7_ZVAL_UNDEF(self->name);
  PHP5TO7_ZVAL_UNDEF(self->options);
  PHP5TO7_ZVAL_UNDEF(self->partition_key);
  PHP5TO7_ZVAL_UNDEF(self->primary_key);
  PHP5TO7_ZVAL_UNDEF(self->clustering_key);
  PHP5TO7_ZVAL_UNDEF(self->clustering_order);

  self->meta   = NULL;
  self->schema = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(table, default_table, self, ce);
}

void php_driver_define_DefaultTable(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\DefaultTable", php_driver_default_table_methods);
  php_driver_default_table_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_default_table_ce TSRMLS_CC, 1, php_driver_table_ce);
  php_driver_default_table_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_default_table_ce->create_object = php_driver_default_table_new;

  memcpy(&php_driver_default_table_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_default_table_handlers.get_properties  = php_driver_default_table_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_default_table_handlers.get_gc          = php_driver_type_default_table_gc;
#endif
  PHP7TO8_COMPARE(php_driver_default_table_handlers, php_driver_default_table_compare);
  php_driver_default_table_handlers.clone_obj = NULL;
}
