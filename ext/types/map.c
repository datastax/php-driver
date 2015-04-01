#include "../php_cassandra.h"
#include "util/collections.h"
#include "map.h"

extern zend_class_entry* spl_ce_Countable;
extern zend_class_entry* zend_ce_iterator;
extern zend_class_entry* zend_ce_arrayaccess;
extern zend_class_entry* cassandra_ce_RuntimeException;
extern zend_class_entry* cassandra_ce_InvalidArgumentException;

zend_class_entry *cassandra_ce_Map = NULL;

typedef struct {
  zval* key;
  zval* value;
} cassandra_map_entry;

int
php_cassandra_map_set(cassandra_map* map, zval* zkey, zval* zvalue TSRMLS_DC)
{
  char* key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(zkey, map->key_type, &key, &key_len TSRMLS_CC)) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid key");
    return 0;
  }

  if (!php_cassandra_validate_object(zvalue, map->value_type TSRMLS_CC)) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid value");
    return 0;
  }

  if (zend_hash_update(&map->keys, key, key_len, (void*) &zkey, sizeof(zval*), NULL) == SUCCESS) {
    Z_ADDREF_P(zkey);
    zend_hash_update(&map->values, key, key_len, (void*) &zvalue, sizeof(zval*), NULL);
    Z_ADDREF_P(zvalue);
    result = 1;
  }

  efree(key);
  return result;
}

static int
php_cassandra_map_get(cassandra_map* map, zval* zkey, zval** zvalue)
{
  char* key;
  int   key_len;
  int   result = 0;
  zval** value;

  if (!php_cassandra_hash_object(zkey, map->key_type, &key, &key_len TSRMLS_CC)) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid key");
    return 0;
  }

  if (zend_hash_find(&map->values, key, key_len, (void**) &value) == SUCCESS) {
    *zvalue = *value;
    result = 1;
  }

  efree(key);
  return result;
}

static int
php_cassandra_map_del(cassandra_map* map, zval* zkey)
{
  char* key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(zkey, map->key_type, &key, &key_len TSRMLS_CC)) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid key");
    return 0;
  }

  if (zend_hash_del(&map->values, key, key_len) == SUCCESS) {
    zend_hash_del(&map->keys, key, key_len);
    result = 1;
  }

  efree(key);
  return result;
}

static int
php_cassandra_map_has(cassandra_map* map, zval* zkey)
{
  char* key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(zkey, map->key_type, &key, &key_len TSRMLS_CC))
    return 0;

  result = zend_hash_exists(&map->keys, key, key_len);

  efree(key);
  return result;
}

static void
php_cassandra_map_populate(cassandra_map* map, zval* keys_array, zval* values_array)
{
  HashPointer values_ptr, keys_ptr;
  zval** current;

  if (values_array) {
    zend_hash_get_pointer(&map->values, &values_ptr);
    zend_hash_internal_pointer_reset(&map->values);

    if (keys_array) {
      zend_hash_get_pointer(&map->keys, &keys_ptr);
      zend_hash_internal_pointer_reset(&map->keys);
    }

    while (zend_hash_get_current_data(&map->values, (void**) &current) == SUCCESS) {
      if (add_next_index_zval(values_array, *current) == SUCCESS) {
        Z_ADDREF_PP(current);

        if (keys_array) {
          zend_hash_get_current_data(&map->keys, (void**) &current);
          add_next_index_zval(keys_array, *current);
          Z_ADDREF_PP(current);
        }
      } else {
        break;
      }

      zend_hash_move_forward(&map->values);

      if (keys_array)
        zend_hash_move_forward(&map->keys);
    }

    zend_hash_set_pointer(&map->values, &values_ptr);

    if (keys_array)
      zend_hash_set_pointer(&map->keys, &keys_ptr);
  } else if (keys_array) {
    zend_hash_get_pointer(&map->keys, &keys_ptr);
    zend_hash_internal_pointer_reset(&map->keys);

    while (zend_hash_get_current_data(&map->keys, (void**) &current) == SUCCESS) {
      if (add_next_index_zval(keys_array, *current) == SUCCESS)
        Z_ADDREF_PP(current);
      else
        break;

      zend_hash_move_forward(&map->keys);
    }

    zend_hash_set_pointer(&map->keys, &keys_ptr);
  }
}

/* {{{ Cassandra\Map::__construct(string) */
PHP_METHOD(CassandraMap, __construct)
{
  char* key_type;
  char* value_type;
  int key_type_len, value_type_len;
  cassandra_map* map;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key_type, &key_type_len, &value_type, &value_type_len) == FAILURE) {
    return;
  }

  map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (!php_cassandra_value_type(key_type, &map->key_type))
    return;
  php_cassandra_value_type(value_type, &map->value_type);
}
/* }}} */

/* {{{ Cassandra\Map::keyType() */
PHP_METHOD(CassandraMap, keyType)
{
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_STRING(php_cassandra_type_name(map->key_type), 1);
}
/* }}} */

/* {{{ Cassandra\Map::valueType() */
PHP_METHOD(CassandraMap, valueType)
{
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_STRING(php_cassandra_type_name(map->value_type), 1);
}
/* }}} */

PHP_METHOD(CassandraMap, keys)
{
  array_init(return_value);
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  php_cassandra_map_populate(map, return_value, NULL);
}

PHP_METHOD(CassandraMap, values)
{
  array_init(return_value);
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  php_cassandra_map_populate(map, NULL, return_value);
}

PHP_METHOD(CassandraMap, set)
{
  zval* key;
  zval* value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &key, &value) == FAILURE)
    return;

  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_map_set(map, key, value TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(CassandraMap, get)
{
  zval* key;
  zval* value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_map_get(map, key, &value))
    RETURN_ZVAL(value, 1, 0);
}

PHP_METHOD(CassandraMap, remove)
{
  zval* key;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_map_del(map, key))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(CassandraMap, has)
{
  zval* key;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_map_has(map, key))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(CassandraMap, count)
{
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(zend_hash_num_elements(&map->values));
}

PHP_METHOD(CassandraMap, current)
{
  zval** current;
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_get_current_data(&map->values, (void**) &current) == SUCCESS)
    RETURN_ZVAL(*current, 1, 0);
}

PHP_METHOD(CassandraMap, key)
{
  zval** current;
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_get_current_data(&map->keys, (void**) &current) == SUCCESS)
    RETURN_ZVAL(*current, 1, 0);
}

PHP_METHOD(CassandraMap, next)
{
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  if (zend_hash_move_forward(&map->keys) == SUCCESS)
    zend_hash_move_forward(&map->values);
}

PHP_METHOD(CassandraMap, valid)
{
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_hash_has_more_elements(&map->values) == SUCCESS);
}

PHP_METHOD(CassandraMap, rewind)
{
  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  zend_hash_internal_pointer_reset(&map->values);
  zend_hash_internal_pointer_reset(&map->keys);
}

PHP_METHOD(CassandraMap, offsetSet)
{
  zval* key;
  zval* value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &key, &value) == FAILURE)
    return;

  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  php_cassandra_map_set(map, key, value TSRMLS_CC);
}

PHP_METHOD(CassandraMap, offsetGet)
{
  zval* key;
  zval* value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_map_get(map, key, &value))
    RETURN_ZVAL(value, 1, 0);
}

PHP_METHOD(CassandraMap, offsetUnset)
{
  zval* key;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  php_cassandra_map_del(map, key);
}

PHP_METHOD(CassandraMap, offsetExists)
{
  zval* key;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  cassandra_map* map = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_map_has(map, key))
    RETURN_TRUE;

  RETURN_FALSE;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, type)
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

static zend_function_entry CassandraMap_methods[] = {
  PHP_ME(CassandraMap, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, keyType, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, keys, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, valueType, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, set, arginfo_two, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, get, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, remove, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, has, arginfo_one, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(CassandraMap, count, arginfo_none, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(CassandraMap, current, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, key, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, next, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, valid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, rewind, arginfo_none, ZEND_ACC_PUBLIC)
  /* ArrayAccess */
  PHP_ME(CassandraMap, offsetSet, arginfo_two, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, offsetGet, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, offsetUnset, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraMap, offsetExists, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_map_handlers;

static HashTable*
php_cassandra_map_properties(zval *object TSRMLS_DC)
{
  zval* values;
  zval* keys;

  cassandra_map* map   = (cassandra_map*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*     props = zend_std_get_properties(object TSRMLS_CC);

  MAKE_STD_ZVAL(values);
  MAKE_STD_ZVAL(keys);
  array_init(values);
  array_init(keys);

  php_cassandra_map_populate(map, keys, values);

  zend_hash_update(props, "keys", sizeof("keys"), &keys, sizeof(zval), NULL);
  zend_hash_update(props, "values", sizeof("values"), &values, sizeof(zval), NULL);

  return props;
}

int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC);

static int
php_cassandra_map_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_map* map1 = (cassandra_map*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_map* map2 = (cassandra_map*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (!(map1->key_type == map2->key_type && map1->value_type == map2->value_type))
    return 1;

  if (zend_compare_symbol_tables_i(&map1->keys, &map2->keys TSRMLS_CC) == 0 &&
      zend_compare_symbol_tables_i(&map1->values, &map2->values TSRMLS_CC) == 0)
    return 0;

  return 1;
}

static void
php_cassandra_map_free(void *object TSRMLS_DC)
{
  cassandra_map* map = (cassandra_map*) object;

  zend_hash_destroy(&map->keys);
  zend_hash_destroy(&map->values);
  zend_object_std_dtor(&map->zval TSRMLS_CC);

  efree(map);
}

static zend_object_value
php_cassandra_map_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_map *map;

  map = (cassandra_map*) emalloc(sizeof(cassandra_map));
  memset(map, 0, sizeof(cassandra_map));

  zend_hash_init(&map->keys, 0, NULL, ZVAL_PTR_DTOR, 0);
  zend_hash_init(&map->values, 0, NULL, ZVAL_PTR_DTOR, 0);
  zend_object_std_init(&map->zval, class_type TSRMLS_CC);
  object_properties_init(&map->zval, class_type TSRMLS_CC);

  retval.handle   = zend_objects_store_put(map,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_map_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_map_handlers;

  return retval;
}

void cassandra_define_CassandraMap(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Map", CassandraMap_methods);
  cassandra_ce_Map = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_map_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_map_handlers.get_properties = php_cassandra_map_properties;
  cassandra_map_handlers.compare_objects = php_cassandra_map_compare;
  cassandra_ce_Map->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Map->create_object = php_cassandra_map_new;
  zend_class_implements(cassandra_ce_Map TSRMLS_CC, 3, spl_ce_Countable, zend_ce_iterator, zend_ce_arrayaccess);
}
