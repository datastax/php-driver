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
#include "DefaultIndex.h"
#include "DefaultTable.h"
#include "Table.h"

zend_class_entry *php_driver_default_materialized_view_ce = NULL;

static void
populate_partition_key(php_driver_materialized_view *view, zval *result TSRMLS_DC)
{
  size_t i, count = cass_materialized_view_meta_partition_key_count(view->meta);
  for (i = 0; i < count; ++i) {
    const CassColumnMeta *column =
        cass_materialized_view_meta_partition_key(view->meta, i);
    if (column) {
      php5to7_zval zcolumn = php_driver_create_column(view->schema, column TSRMLS_CC);
      if (!PHP5TO7_ZVAL_IS_UNDEF(zcolumn)) {
        add_next_index_zval(result, PHP5TO7_ZVAL_MAYBE_P(zcolumn));
      }
    }
  }
}

static void
populate_clustering_key(php_driver_materialized_view *view, zval *result TSRMLS_DC)
{
  size_t i, count = cass_materialized_view_meta_clustering_key_count(view->meta);
  for (i = 0; i < count; ++i) {
    const CassColumnMeta *column =
        cass_materialized_view_meta_clustering_key(view->meta, i);
    if (column) {
      php5to7_zval zcolumn = php_driver_create_column(view->schema, column TSRMLS_CC);
      if (!PHP5TO7_ZVAL_IS_UNDEF(zcolumn)) {
        add_next_index_zval(result, PHP5TO7_ZVAL_MAYBE_P(zcolumn));
      }
    }
  }
}

php5to7_zval
php_driver_create_materialized_view(php_driver_ref* schema,
                                       const CassMaterializedViewMeta *meta TSRMLS_DC)
{
  php5to7_zval result;
  php_driver_materialized_view *view;
  const char *name;
  size_t name_length;

  PHP5TO7_ZVAL_UNDEF(result);

  PHP5TO7_ZVAL_MAYBE_MAKE(result);
  object_init_ex(PHP5TO7_ZVAL_MAYBE_P(result), php_driver_default_materialized_view_ce);

  view = PHP_DRIVER_GET_MATERIALIZED_VIEW(PHP5TO7_ZVAL_MAYBE_P(result));
  view->schema = php_driver_add_ref(schema);
  view->meta   = meta;

  cass_materialized_view_meta_name(meta, &name, &name_length);
  PHP5TO7_ZVAL_MAYBE_MAKE(view->name);
  PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(view->name), name, name_length);

  return result;
}

void
php_driver_default_materialized_view_build_options(php_driver_materialized_view *view TSRMLS_DC) {
  CassIterator *iterator =
      cass_iterator_fields_from_materialized_view_meta(view->meta);
  view->options =
      php_driver_table_build_options(iterator TSRMLS_CC);
  cass_iterator_free(iterator);
}

void
php_driver_materialized_view_get_option(php_driver_materialized_view *view,
                                           const char *name,
                                           zval *result TSRMLS_DC) {
  zval *zvalue;
  if (PHP5TO7_ZVAL_IS_UNDEF(view->options)) {
    php_driver_default_materialized_view_build_options(view TSRMLS_CC);
  }

  if (!PHP5TO7_ZEND_HASH_FIND(PHP5TO7_Z_ARRVAL_MAYBE_P(view->options),
                         name, strlen(name) + 1,
                         zvalue)) {
    ZVAL_FALSE(result);
    return;
  }

  PHP5TO7_ZVAL_COPY(result, zvalue);
}

PHP_METHOD(DefaultMaterializedView, name)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->name), 1, 0);
}

PHP_METHOD(DefaultMaterializedView, option)
{
  char *name;
  php5to7_size name_len;
  php_driver_materialized_view *self;
  php5to7_zval* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->options)) {
    php_driver_default_materialized_view_build_options(self TSRMLS_CC);
  }

  if (PHP5TO7_ZEND_HASH_FIND(PHP5TO7_Z_ARRVAL_MAYBE_P(self->options),
                         name, name_len + 1,
                         result)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(result), 1, 0);
  }
  RETURN_FALSE;
}

PHP_METHOD(DefaultMaterializedView, options)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->options)) {
    php_driver_default_materialized_view_build_options(self TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->options), 1, 0);
}

PHP_METHOD(DefaultMaterializedView, comment)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "comment", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, readRepairChance)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "read_repair_chance", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, localReadRepairChance)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "local_read_repair_chance", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, gcGraceSeconds)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "gc_grace_seconds", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, caching)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "caching", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, bloomFilterFPChance)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "bloom_filter_fp_chance", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, memtableFlushPeriodMs)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "memtable_flush_period_in_ms", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, defaultTTL)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "default_time_to_live", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, speculativeRetry)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "speculative_retry", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, indexInterval)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "index_interval", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, compactionStrategyClassName)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "compaction_strategy_class", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, compactionStrategyOptions)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "compaction_strategy_options", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, compressionParameters)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "compression_parameters", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, populateIOCacheOnFlush)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "populate_io_cache_on_flush", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, replicateOnWrite)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "replicate_on_write", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, maxIndexInterval)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "max_index_interval", return_value TSRMLS_CC);
}

PHP_METHOD(DefaultMaterializedView, minIndexInterval)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());

  php_driver_materialized_view_get_option(self, "min_index_interval", return_value TSRMLS_CC);
}


PHP_METHOD(DefaultMaterializedView, column)
{
  php_driver_materialized_view *self;
  char *name;
  php5to7_size name_len;
  php5to7_zval column;
  const CassColumnMeta *meta;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  meta = cass_materialized_view_meta_column_by_name(self->meta, name);
  if (meta == NULL) {
    RETURN_FALSE;
  }

  column = php_driver_create_column(self->schema, meta TSRMLS_CC);
  if (PHP5TO7_ZVAL_IS_UNDEF(column)) {
    return;
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(column), 0, 1);
}

PHP_METHOD(DefaultMaterializedView, columns)
{
  php_driver_materialized_view *self;
  CassIterator    *iterator;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self     = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  iterator = cass_iterator_columns_from_materialized_view_meta(self->meta);

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

PHP_METHOD(DefaultMaterializedView, partitionKey)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->partition_key)) {
    PHP5TO7_ZVAL_MAYBE_MAKE(self->partition_key);
    array_init(PHP5TO7_ZVAL_MAYBE_P(self->partition_key));
    populate_partition_key(self, PHP5TO7_ZVAL_MAYBE_P(self->partition_key) TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->partition_key), 1, 0);
}

PHP_METHOD(DefaultMaterializedView, primaryKey)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->primary_key)) {
    PHP5TO7_ZVAL_MAYBE_MAKE(self->primary_key);
    array_init(PHP5TO7_ZVAL_MAYBE_P(self->primary_key));
    populate_partition_key(self, PHP5TO7_ZVAL_MAYBE_P(self->primary_key) TSRMLS_CC);
    populate_clustering_key(self, PHP5TO7_ZVAL_MAYBE_P(self->primary_key) TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->primary_key), 1, 0);
}

PHP_METHOD(DefaultMaterializedView, clusteringKey)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->clustering_key)) {
    PHP5TO7_ZVAL_MAYBE_MAKE(self->clustering_key);
    array_init(PHP5TO7_ZVAL_MAYBE_P(self->clustering_key));
    populate_clustering_key(self, PHP5TO7_ZVAL_MAYBE_P(self->clustering_key) TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->clustering_key), 1, 0);
}

PHP_METHOD(DefaultMaterializedView, clusteringOrder)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->clustering_order)) {
    size_t i, count = cass_materialized_view_meta_clustering_key_count(self->meta);
    PHP5TO7_ZVAL_MAYBE_MAKE(self->clustering_order);
    array_init(PHP5TO7_ZVAL_MAYBE_P(self->clustering_order));
    for (i = 0; i < count; ++i) {
      CassClusteringOrder order =
          cass_materialized_view_meta_clustering_key_order(self->meta, i);
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

PHP_METHOD(DefaultMaterializedView, baseTable)
{
  php_driver_materialized_view *self;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_MATERIALIZED_VIEW(getThis());
  if (PHP5TO7_ZVAL_IS_UNDEF(self->base_table)) {
    const CassTableMeta *table =
        cass_materialized_view_meta_base_table(self->meta);
    if (!table) {
      return;
    }
    self->base_table = php_driver_create_table(self->schema,
                                                  table TSRMLS_CC);
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->base_table), 1, 0);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_default_materialized_view_methods[] = {
  PHP_ME(DefaultMaterializedView, name, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, option, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, options, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, comment, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, readRepairChance, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, localReadRepairChance, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, gcGraceSeconds, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, caching, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, bloomFilterFPChance, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, memtableFlushPeriodMs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, defaultTTL, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, speculativeRetry, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, indexInterval, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, compactionStrategyClassName, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, compactionStrategyOptions, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, compressionParameters, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, populateIOCacheOnFlush, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, replicateOnWrite, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, maxIndexInterval, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, minIndexInterval, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, column, arginfo_name, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, columns, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, partitionKey, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, primaryKey, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, clusteringKey, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, clusteringOrder, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultMaterializedView, baseTable, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_default_materialized_view_handlers;

static HashTable *
php_driver_type_default_materialized_view_gc(php7to8_object *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_default_materialized_view_properties(php7to8_object *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_default_materialized_view_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_default_materialized_view_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_materialized_view *self = PHP5TO7_ZEND_OBJECT_GET(materialized_view, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->name);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->options);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->partition_key);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->primary_key);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->clustering_key);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->clustering_order);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->base_table);

  if (self->schema) {
    php_driver_del_ref(&self->schema);
    self->schema = NULL;
  }
  self->meta = NULL;

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_default_materialized_view_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_materialized_view *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(materialized_view, ce);

  PHP5TO7_ZVAL_UNDEF(self->name);
  PHP5TO7_ZVAL_UNDEF(self->options);
  PHP5TO7_ZVAL_UNDEF(self->partition_key);
  PHP5TO7_ZVAL_UNDEF(self->primary_key);
  PHP5TO7_ZVAL_UNDEF(self->clustering_key);
  PHP5TO7_ZVAL_UNDEF(self->clustering_order);
  PHP5TO7_ZVAL_UNDEF(self->base_table);

  self->meta   = NULL;
  self->schema = NULL;

  PHP5TO7_ZEND_OBJECT_INIT_EX(materialized_view, default_materialized_view, self, ce);
}

void php_driver_define_DefaultMaterializedView(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\DefaultMaterializedView", php_driver_default_materialized_view_methods);
  php_driver_default_materialized_view_ce = php5to7_zend_register_internal_class_ex(&ce, php_driver_materialized_view_ce);
  php_driver_default_materialized_view_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_default_materialized_view_ce->create_object = php_driver_default_materialized_view_new;

  memcpy(&php_driver_default_materialized_view_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_default_materialized_view_handlers.get_properties  = php_driver_default_materialized_view_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_default_materialized_view_handlers.get_gc          = php_driver_type_default_materialized_view_gc;
#endif
  PHP7TO8_COMPARE(php_driver_default_materialized_view_handlers, php_driver_default_materialized_view_compare);
  php_driver_default_materialized_view_handlers.clone_obj = NULL;
}
