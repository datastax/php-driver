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
#include "util/collections.h"
#include "util/hash.h"
#include "util/types.h"
#include "Map.h"

zend_class_entry *php_driver_map_ce = NULL;

int
php_driver_map_set(php_driver_map *map, zval *zkey, zval *zvalue TSRMLS_DC)
{
  php_driver_map_entry *entry;
  php_driver_type *type;

  if (Z_TYPE_P(zkey) == IS_NULL) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid key: null is not supported inside maps");
    return 0;
  }

  if (Z_TYPE_P(zvalue) == IS_NULL) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid value: null is not supported inside maps");
    return 0;
  }

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map->type));

  if (!php_driver_validate_object(zkey, PHP5TO7_ZVAL_MAYBE_P(type->data.map.key_type) TSRMLS_CC)) {
    return 0;
  }

  if (!php_driver_validate_object(zvalue, PHP5TO7_ZVAL_MAYBE_P(type->data.map.value_type) TSRMLS_CC)) {
    return 0;
  }

  map->dirty = 1;
  HASH_FIND_ZVAL(map->entries, zkey, entry);
  if (entry == NULL) {
    entry = (php_driver_map_entry *) emalloc(sizeof(php_driver_map_entry));
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(entry->key), zkey);
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(entry->value), zvalue);
    HASH_ADD_ZVAL(map->entries, key, entry);
  } else {
    php5to7_zval prev_value = entry->value;
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(entry->value), zvalue);
    zval_ptr_dtor(&prev_value);
  }

  return 1;
}

static int
php_driver_map_get(php_driver_map *map, zval *zkey, php5to7_zval *zvalue TSRMLS_DC)
{
  php_driver_map_entry *entry;
  php_driver_type *type;
  int result = 0;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map->type));

  if (!php_driver_validate_object(zkey, PHP5TO7_ZVAL_MAYBE_P(type->data.map.key_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(map->entries, zkey, entry);
  if (entry != NULL) {
    *zvalue = entry->value;
    result = 1;
  }

  return result;
}

static int
php_driver_map_del(php_driver_map *map, zval *zkey TSRMLS_DC)
{
  php_driver_map_entry *entry;
  php_driver_type *type;
  int result = 0;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map->type));

  if (!php_driver_validate_object(zkey, PHP5TO7_ZVAL_MAYBE_P(type->data.map.key_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(map->entries, zkey, entry);
  if (entry != NULL) {
    map->dirty = 1;
    if (entry == map->iter_temp) {
      map->iter_temp = (php_driver_map_entry *)map->iter_temp->hh.next;
    }
    HASH_DEL(map->entries, entry);
    zval_ptr_dtor(&entry->key);
    zval_ptr_dtor(&entry->value);
    efree(entry);
    result = 1;
  }

  return result;
}

static int
php_driver_map_has(php_driver_map *map, zval *zkey TSRMLS_DC)
{
  php_driver_map_entry *entry;
  php_driver_type *type;
  int result = 0;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map->type));

  if (!php_driver_validate_object(zkey, PHP5TO7_ZVAL_MAYBE_P(type->data.map.key_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(map->entries, zkey, entry);
  if (entry != NULL) {
    result = 1;
  }

  return result;
}

static void
php_driver_map_populate_keys(const php_driver_map *map, zval *array TSRMLS_DC)
{
  php_driver_map_entry *curr,  *temp;
  HASH_ITER(hh, map->entries, curr, temp) {
    if (add_next_index_zval(array, PHP5TO7_ZVAL_MAYBE_P(curr->key)) != SUCCESS) {
      break;
    }
    Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(curr->key));
  }
}

static void
php_driver_map_populate_values(const php_driver_map *map, zval *array TSRMLS_DC)
{
  php_driver_map_entry *curr, *temp;
  HASH_ITER(hh, map->entries, curr, temp) {
    if (add_next_index_zval(array, PHP5TO7_ZVAL_MAYBE_P(curr->value)) != SUCCESS) {
      break;
    }
    Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(curr->value));
  }
}

/* {{{ Map::__construct(type, type) */
PHP_METHOD(Map, __construct)
{
  php_driver_map *self;
  zval *key_type;
  zval *value_type;
  php5to7_zval scalar_key_type;
  php5to7_zval scalar_value_type;

  PHP5TO7_ZVAL_UNDEF(scalar_key_type);
  PHP5TO7_ZVAL_UNDEF(scalar_value_type);

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &key_type, &value_type) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  if (Z_TYPE_P(key_type) == IS_STRING) {
    CassValueType type;
    if (!php_driver_value_type(Z_STRVAL_P(key_type), &type TSRMLS_CC))
      return;
    scalar_key_type = php_driver_type_scalar(type TSRMLS_CC);
    key_type = PHP5TO7_ZVAL_MAYBE_P(scalar_key_type);
  } else if (Z_TYPE_P(key_type) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(key_type), php_driver_type_ce TSRMLS_CC)) {
    if (!php_driver_type_validate(key_type, "keyType" TSRMLS_CC)) {
      return;
    }
    Z_ADDREF_P(key_type);
  } else {
    throw_invalid_argument(key_type,
                           "keyType",
                           "a string or an instance of " PHP_DRIVER_NAMESPACE "\\Type" TSRMLS_CC);
    return;
  }

  if (Z_TYPE_P(value_type) == IS_STRING) {
    CassValueType type;
    if (!php_driver_value_type(Z_STRVAL_P(value_type), &type TSRMLS_CC))
      return;
    scalar_value_type = php_driver_type_scalar(type TSRMLS_CC);
    value_type = PHP5TO7_ZVAL_MAYBE_P(scalar_value_type);
  } else if (Z_TYPE_P(value_type) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(value_type), php_driver_type_ce TSRMLS_CC)) {
    if (!php_driver_type_validate(value_type, "valueType" TSRMLS_CC)) {
      return;
    }
    Z_ADDREF_P(value_type);
  } else {
    zval_ptr_dtor(PHP5TO7_ZVAL_MAYBE_ADDR_OF(key_type));
    throw_invalid_argument(value_type,
                           "valueType",
                           "a string or an instance of " PHP_DRIVER_NAMESPACE "\\Type" TSRMLS_CC);
    return;
  }

  self->type = php_driver_type_map(key_type, value_type TSRMLS_CC);
}
/* }}} */

/* {{{ Map::type() */
PHP_METHOD(Map, type)
{
  php_driver_map *self = PHP_DRIVER_GET_MAP(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->type), 1, 0);
}
/* }}} */

PHP_METHOD(Map, keys)
{
  php_driver_map *self = NULL;
  array_init(return_value);
  self = PHP_DRIVER_GET_MAP(getThis());
  php_driver_map_populate_keys(self, return_value TSRMLS_CC);
}

PHP_METHOD(Map, values)
{
  php_driver_map *self = NULL;
  array_init(return_value);
  self = PHP_DRIVER_GET_MAP(getThis());
  php_driver_map_populate_values(self, return_value TSRMLS_CC);
}

PHP_METHOD(Map, set)
{
  zval *key;
  php_driver_map *self = NULL;
  zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &key, &value) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  if (php_driver_map_set(self, key, value TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(Map, get)
{
  zval *key;
  php_driver_map *self = NULL;
  php5to7_zval value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  if (php_driver_map_get(self, key, &value TSRMLS_CC))
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 1, 0);
}

PHP_METHOD(Map, remove)
{
  zval *key;
  php_driver_map *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  if (php_driver_map_del(self, key TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(Map, has)
{
  zval *key;
  php_driver_map *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  if (php_driver_map_has(self, key TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(Map, count)
{
  php_driver_map *self = PHP_DRIVER_GET_MAP(getThis());
  RETURN_LONG((long)HASH_COUNT(self->entries));
}

PHP_METHOD(Map, current)
{
  php_driver_map *self = PHP_DRIVER_GET_MAP(getThis());
  if (self->iter_curr != NULL)
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->iter_curr->value), 1, 0);
}

PHP_METHOD(Map, key)
{
  php_driver_map *self = PHP_DRIVER_GET_MAP(getThis());
  if (self->iter_curr != NULL)
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->iter_curr->key), 1, 0);
}

PHP_METHOD(Map, next)
{
  php_driver_map *self = PHP_DRIVER_GET_MAP(getThis());
  self->iter_curr = self->iter_temp;
  self->iter_temp = self->iter_temp != NULL ? (php_driver_map_entry *)self->iter_temp->hh.next : NULL;
}

PHP_METHOD(Map, valid)
{
  php_driver_map *self = PHP_DRIVER_GET_MAP(getThis());
  RETURN_BOOL(self->iter_curr != NULL);
}

PHP_METHOD(Map, rewind)
{
  php_driver_map *self = PHP_DRIVER_GET_MAP(getThis());
  self->iter_curr = self->entries;
  self->iter_temp = self->entries != NULL ? (php_driver_map_entry *)self->entries->hh.next : NULL;
}

PHP_METHOD(Map, offsetSet)
{
  zval *key;
  php_driver_map *self = NULL;
  zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &key, &value) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  php_driver_map_set(self, key, value TSRMLS_CC);
}

PHP_METHOD(Map, offsetGet)
{
  zval *key;
  php_driver_map *self = NULL;
  php5to7_zval value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  if (php_driver_map_get(self, key, &value TSRMLS_CC))
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 1, 0);
}

PHP_METHOD(Map, offsetUnset)
{
  zval *key;
  php_driver_map *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  php_driver_map_del(self, key TSRMLS_CC);
}

PHP_METHOD(Map, offsetExists)
{
  zval *key;
  php_driver_map *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  self = PHP_DRIVER_GET_MAP(getThis());

  if (php_driver_map_has(self, key TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, keyType)
  ZEND_ARG_INFO(0, valueType)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_two, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_one, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

PHP7TO8_ARG_INFO_VOID_RETURN(arginfo_void_return)
PHP7TO8_ARG_INFO_BOOL_RETURN(arginfo_bool_return)
PHP7TO8_ARG_INFO_MIXED_RETURN(arginfo_mixed_return)
PHP7TO8_ARG_INFO_LONG_RETURN(arginfo_long_return)

PHP7TO8_ARG_INFO_MIXED_BOOL_RETURN(arginfo_mixed_bool_return, offset)
PHP7TO8_ARG_INFO_MIXED_MIXED_RETURN(arginfo_mixed_mixed_return, offset)
PHP7TO8_ARG_INFO_MIXED_MIXED_VOID_RETURN(arginfo_mixed_mixed_void_return, offset, value)
PHP7TO8_ARG_INFO_MIXED_VOID_RETURN(arginfo_mixed_void_return, offset)

static zend_function_entry php_driver_map_methods[] = {
  PHP_ME(Map, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Map, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, keys, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, set, arginfo_two, ZEND_ACC_PUBLIC)
  PHP_ME(Map, get, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Map, remove, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Map, has, arginfo_one, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(Map, count, arginfo_long_return, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(Map, current, arginfo_mixed_return, ZEND_ACC_PUBLIC)
  PHP_ME(Map, key, arginfo_mixed_return, ZEND_ACC_PUBLIC)
  PHP_ME(Map, next, arginfo_void_return, ZEND_ACC_PUBLIC)
  PHP_ME(Map, rewind, arginfo_void_return, ZEND_ACC_PUBLIC)
  PHP_ME(Map, valid, arginfo_bool_return, ZEND_ACC_PUBLIC)
  /* ArrayAccess */
  PHP_ME(Map, offsetExists, arginfo_mixed_bool_return, ZEND_ACC_PUBLIC)
  PHP_ME(Map, offsetGet, arginfo_mixed_mixed_return, ZEND_ACC_PUBLIC)
  PHP_ME(Map, offsetSet, arginfo_mixed_mixed_void_return, ZEND_ACC_PUBLIC)
  PHP_ME(Map, offsetUnset, arginfo_mixed_void_return, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_map_handlers;

static HashTable *
php_driver_map_gc(php7to8_object *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_map_properties(php7to8_object *object TSRMLS_DC)
{
  php5to7_zval keys;
  php5to7_zval values;

#if PHP_MAJOR_VERSION >= 8
  php_driver_map *self = PHP5TO7_ZEND_OBJECT_GET(map, object);
#else
  php_driver_map *self = PHP_DRIVER_GET_MAP(object);
#endif
  HashTable     *props = zend_std_get_properties(object TSRMLS_CC);


  PHP5TO7_ZEND_HASH_UPDATE(props,
                          "type", sizeof("type"),
                           PHP5TO7_ZVAL_MAYBE_P(self->type), sizeof(zval));
  Z_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->type));

  PHP5TO7_ZVAL_MAYBE_MAKE(keys);
  array_init(PHP5TO7_ZVAL_MAYBE_P(keys));
  php_driver_map_populate_keys(self, PHP5TO7_ZVAL_MAYBE_P(keys) TSRMLS_CC);
  PHP5TO7_ZEND_HASH_SORT(Z_ARRVAL_P(PHP5TO7_ZVAL_MAYBE_P(keys)), php_driver_data_compare, 1);
  PHP5TO7_ZEND_HASH_UPDATE(props, "keys", sizeof("keys"), PHP5TO7_ZVAL_MAYBE_P(keys), sizeof(zval *));

  PHP5TO7_ZVAL_MAYBE_MAKE(values);
  array_init(PHP5TO7_ZVAL_MAYBE_P(values));
  php_driver_map_populate_values(self, PHP5TO7_ZVAL_MAYBE_P(values) TSRMLS_CC);
  PHP5TO7_ZEND_HASH_SORT(Z_ARRVAL_P(PHP5TO7_ZVAL_MAYBE_P(values)), php_driver_data_compare, 1);
  PHP5TO7_ZEND_HASH_UPDATE(props, "values", sizeof("values"), PHP5TO7_ZVAL_MAYBE_P(values), sizeof(zval *));

  return props;
}

static int
php_driver_map_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  php_driver_map_entry *curr, *temp;
  php_driver_map *map1;
  php_driver_map *map2;
  php_driver_type *type1;
  php_driver_type *type2;
  int result;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  map1 = PHP_DRIVER_GET_MAP(obj1);
  map2 = PHP_DRIVER_GET_MAP(obj2);

  type1 = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map1->type));
  type2 = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map2->type));

  result = php_driver_type_compare(type1, type2 TSRMLS_CC);
  if (result != 0) return result;

  if (HASH_COUNT(map1->entries) != HASH_COUNT(map2->entries)) {
   return HASH_COUNT(map1->entries) < HASH_COUNT(map2->entries) ? -1 : 1;
  }

  HASH_ITER(hh, map1->entries, curr, temp) {
    php_driver_map_entry *entry = NULL;
    HASH_FIND_ZVAL(map2->entries, PHP5TO7_ZVAL_MAYBE_P(curr->key), entry);
    if (entry == NULL) {
      return 1;
    }
    result = php_driver_value_compare(PHP5TO7_ZVAL_MAYBE_P(curr->value),
                                      PHP5TO7_ZVAL_MAYBE_P(entry->value) TSRMLS_CC);
    if (result != 0) return result;
  }

  return 0;
}

static unsigned
php_driver_map_hash_value(zval *obj TSRMLS_DC)
{
  php_driver_map *self = PHP_DRIVER_GET_MAP(obj);
  php_driver_map_entry *curr, *temp;
  unsigned hashv = 0;

  if (!self->dirty) return self->hashv;

  HASH_ITER(hh, self->entries, curr, temp) {
    hashv = php_driver_combine_hash(hashv,
                                       php_driver_value_hash(PHP5TO7_ZVAL_MAYBE_P(curr->key) TSRMLS_CC));
    hashv = php_driver_combine_hash(hashv,
                                       php_driver_value_hash(PHP5TO7_ZVAL_MAYBE_P(curr->value) TSRMLS_CC));
  }

  self->hashv = hashv;
  self->dirty = 0;

  return hashv;
}

static void
php_driver_map_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_map *self = PHP5TO7_ZEND_OBJECT_GET(map, object);
  php_driver_map_entry *curr, *temp;

  HASH_ITER(hh, self->entries, curr, temp) {
    zval_ptr_dtor(&curr->key);
    zval_ptr_dtor(&curr->value);
    HASH_DEL(self->entries, curr);
    efree(curr);
  }

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->type);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_map_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_map *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(map, ce);

  self->entries = self->iter_curr = self->iter_temp = NULL;
  self->dirty = 1;
  PHP5TO7_ZVAL_UNDEF(self->type);

  PHP5TO7_ZEND_OBJECT_INIT(map, self, ce);
}

void php_driver_define_Map(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Map", php_driver_map_methods);
  php_driver_map_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_map_ce TSRMLS_CC, 1, php_driver_value_ce);
  memcpy(&php_driver_map_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_map_handlers.std.get_properties  = php_driver_map_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_map_handlers.std.get_gc          = php_driver_map_gc;
#endif
  PHP7TO8_COMPARE(php_driver_map_handlers.std, php_driver_map_compare);
  php_driver_map_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_map_ce->create_object = php_driver_map_new;
  zend_class_implements(php_driver_map_ce TSRMLS_CC, 3, PHP7TO8_COUNTABLE, zend_ce_iterator, zend_ce_arrayaccess);

  php_driver_map_handlers.hash_value = php_driver_map_hash_value;
  php_driver_map_handlers.std.clone_obj = NULL;
}
