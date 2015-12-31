#include "php_cassandra.h"
#include "util/collections.h"
#include "Map.h"

zend_class_entry *cassandra_map_ce = NULL;

typedef struct {
  zval *key;
  zval *value;
} cassandra_map_entry;

int
php_cassandra_map_set(cassandra_map *map, zval *zkey, zval *zvalue TSRMLS_DC)
{
  char *key;
  int   key_len;
  int   result = 0;

  if (Z_TYPE_P(zkey) == IS_NULL) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid key: null is not supported inside maps");
    return 0;
  }

  if (Z_TYPE_P(zvalue) == IS_NULL) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid value: null is not supported inside maps");
    return 0;
  }

  if (!php_cassandra_hash_object(zkey, map->key_type, &key, &key_len TSRMLS_CC)) {
    return 0;
  }

  if (!php_cassandra_validate_object(zvalue, map->value_type TSRMLS_CC)) {
    return 0;
  }

  if (PHP5TO7_ZEND_HASH_UPDATE(&map->keys, key, key_len + 1, zkey, sizeof(zval*)) &&
      PHP5TO7_ZEND_HASH_UPDATE(&map->values, key, key_len + 1, zvalue, sizeof(zval*))) {
    Z_TRY_ADDREF_P(zkey);
    Z_TRY_ADDREF_P(zvalue);
    result = 1;
  }

  efree(key);
  return result;
}

static int
php_cassandra_map_get(cassandra_map *map, zval *zkey, php5to7_zval *zvalue TSRMLS_DC)
{
  char *key;
  int   key_len;
  int   result = 0;
  php5to7_zval *value;

  if (!php_cassandra_hash_object(zkey, map->key_type, &key, &key_len TSRMLS_CC)) {
    return 0;
  }

  if (PHP5TO7_ZEND_HASH_FIND(&map->values, key, key_len + 1, value)) {
    *zvalue = *value;
    result = 1;
  }

  efree(key);
  return result;
}

static int
php_cassandra_map_del(cassandra_map *map, zval *zkey TSRMLS_DC)
{
  char *key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(zkey, map->key_type, &key, &key_len TSRMLS_CC)) {
    return 0;
  }

  if (PHP5TO7_ZEND_HASH_DEL(&map->values, key, key_len + 1)) {
    PHP5TO7_ZEND_HASH_DEL(&map->keys, key, key_len + 1);
    result = 1;
  }

  efree(key);
  return result;
}

static int
php_cassandra_map_has(cassandra_map *map, zval *zkey TSRMLS_DC)
{
  char *key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(zkey, map->key_type, &key, &key_len TSRMLS_CC))
    return 0;

  result = PHP5TO7_ZEND_HASH_EXISTS(&map->keys, key, key_len + 1);

  efree(key);
  return result;
}

static void
php_cassandra_map_populate(HashTable *hash, zval *array)
{
  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_VAL(hash, current) {
    if (add_next_index_zval(array, PHP5TO7_ZVAL_MAYBE_DEREF(current)) == SUCCESS)
      Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_DEREF(current));
    else
      break;
  } PHP5TO7_ZEND_HASH_FOREACH_END(hash);
}

/* {{{ Cassandra\Map::__construct(string, string) */
PHP_METHOD(Map, __construct)
{
  char *key_type;
  char *value_type;
  php5to7_size key_type_len, value_type_len;
  cassandra_map *map;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key_type, &key_type_len, &value_type, &value_type_len) == FAILURE) {
    return;
  }

  map = PHP_CASSANDRA_GET_MAP(getThis());

  if (!php_cassandra_value_type(key_type, &map->key_type TSRMLS_CC))
    return;
  php_cassandra_value_type(value_type, &map->value_type TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Map::keyType() */
PHP_METHOD(Map, keyType)
{
  cassandra_map *map = PHP_CASSANDRA_GET_MAP(getThis());

  PHP5TO7_RETVAL_STRING(php_cassandra_type_name(map->key_type));
}
/* }}} */

/* {{{ Cassandra\Map::valueType() */
PHP_METHOD(Map, valueType)
{
  cassandra_map *map = PHP_CASSANDRA_GET_MAP(getThis());

  PHP5TO7_RETVAL_STRING(php_cassandra_type_name(map->value_type));
}
/* }}} */

PHP_METHOD(Map, keys)
{
  cassandra_map *map = NULL;
  array_init(return_value);
  map = PHP_CASSANDRA_GET_MAP(getThis());
  php_cassandra_map_populate(&map->keys, return_value);
}

PHP_METHOD(Map, values)
{
  cassandra_map *map = NULL;
  array_init(return_value);
  map = PHP_CASSANDRA_GET_MAP(getThis());
  php_cassandra_map_populate(&map->values, return_value);
}

PHP_METHOD(Map, set)
{
  zval *key;
  cassandra_map *map = NULL;
  zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &key, &value) == FAILURE)
    return;

  map = PHP_CASSANDRA_GET_MAP(getThis());

  if (php_cassandra_map_set(map, key, value TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(Map, get)
{
  zval *key;
  cassandra_map *map = NULL;
  php5to7_zval value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  map = PHP_CASSANDRA_GET_MAP(getThis());

  if (php_cassandra_map_get(map, key, &value TSRMLS_CC))
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 1, 0);
}

PHP_METHOD(Map, remove)
{
  zval *key;
  cassandra_map *map = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  map = PHP_CASSANDRA_GET_MAP(getThis());

  if (php_cassandra_map_del(map, key TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(Map, has)
{
  zval *key;
  cassandra_map *map = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  map = PHP_CASSANDRA_GET_MAP(getThis());

  if (php_cassandra_map_has(map, key TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(Map, count)
{
  cassandra_map *map = PHP_CASSANDRA_GET_MAP(getThis());
  RETURN_LONG(zend_hash_num_elements(&map->values));
}

PHP_METHOD(Map, current)
{
  php5to7_zval *current;
  cassandra_map *map = PHP_CASSANDRA_GET_MAP(getThis());

  if (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(&map->values, current))
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(current), 1, 0);
}

PHP_METHOD(Map, key)
{
  php5to7_zval *current;
  cassandra_map *map = PHP_CASSANDRA_GET_MAP(getThis());

  if (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(&map->keys, current))
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(current), 1, 0);
}

PHP_METHOD(Map, next)
{
  cassandra_map *map = PHP_CASSANDRA_GET_MAP(getThis());
  if (zend_hash_move_forward(&map->keys) == SUCCESS)
    zend_hash_move_forward(&map->values);
}

PHP_METHOD(Map, valid)
{
  cassandra_map *map = PHP_CASSANDRA_GET_MAP(getThis());
  RETURN_BOOL(zend_hash_has_more_elements(&map->values) == SUCCESS);
}

PHP_METHOD(Map, rewind)
{
  cassandra_map *map = PHP_CASSANDRA_GET_MAP(getThis());
  zend_hash_internal_pointer_reset(&map->values);
  zend_hash_internal_pointer_reset(&map->keys);
}

PHP_METHOD(Map, offsetSet)
{
  zval *key;
  cassandra_map *map = NULL;
  zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &key, &value) == FAILURE)
    return;

  map = PHP_CASSANDRA_GET_MAP(getThis());

  php_cassandra_map_set(map, key, value TSRMLS_CC);
}

PHP_METHOD(Map, offsetGet)
{
  zval *key;
  cassandra_map *map = NULL;
  php5to7_zval value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  map = PHP_CASSANDRA_GET_MAP(getThis());

  if (php_cassandra_map_get(map, key, &value TSRMLS_CC))
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(value), 1, 0);
}

PHP_METHOD(Map, offsetUnset)
{
  zval *key;
  cassandra_map *map = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  map = PHP_CASSANDRA_GET_MAP(getThis());

  php_cassandra_map_del(map, key TSRMLS_CC);
}

PHP_METHOD(Map, offsetExists)
{
  zval *key;
  cassandra_map *map = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key) == FAILURE)
    return;

  map = PHP_CASSANDRA_GET_MAP(getThis());

  if (php_cassandra_map_has(map, key TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
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

static zend_function_entry cassandra_map_methods[] = {
  PHP_ME(Map, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Map, keyType, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, keys, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, valueType, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, set, arginfo_two, ZEND_ACC_PUBLIC)
  PHP_ME(Map, get, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Map, remove, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Map, has, arginfo_one, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(Map, count, arginfo_none, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(Map, current, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, key, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, next, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, valid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Map, rewind, arginfo_none, ZEND_ACC_PUBLIC)
  /* ArrayAccess */
  PHP_ME(Map, offsetSet, arginfo_two, ZEND_ACC_PUBLIC)
  PHP_ME(Map, offsetGet, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Map, offsetUnset, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Map, offsetExists, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_map_handlers;

static HashTable*
php_cassandra_map_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_map_properties(zval *object TSRMLS_DC)
{
  cassandra_map *self = PHP_CASSANDRA_GET_MAP(object);
  HashTable     *props = zend_std_get_properties(object TSRMLS_CC);
  php5to7_zval   values;
  php5to7_zval   keys;


  PHP5TO7_ZVAL_MAYBE_MAKE(values);
  PHP5TO7_ZVAL_MAYBE_MAKE(keys);
  array_init(PHP5TO7_ZVAL_MAYBE_P(values));
  array_init(PHP5TO7_ZVAL_MAYBE_P(keys));
  php_cassandra_map_populate(&self->values, PHP5TO7_ZVAL_MAYBE_P(values));
  php_cassandra_map_populate(&self->keys, PHP5TO7_ZVAL_MAYBE_P(keys));
  PHP5TO7_ZEND_HASH_UPDATE(props, "keys", sizeof("keys"), PHP5TO7_ZVAL_MAYBE_P(keys), sizeof(zval*));
  PHP5TO7_ZEND_HASH_UPDATE(props, "values", sizeof("values"), PHP5TO7_ZVAL_MAYBE_P(values), sizeof(zval*));

  return props;
}

int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC);

static int
php_cassandra_map_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_map *map1 = NULL;
  cassandra_map *map2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  map1 = PHP_CASSANDRA_GET_MAP(obj1);
  map2 = PHP_CASSANDRA_GET_MAP(obj2);

  if (!(map1->key_type == map2->key_type && map1->value_type == map2->value_type))
    return 1;

  if (zend_compare_symbol_tables_i(&map1->keys, &map2->keys TSRMLS_CC) == 0 &&
      zend_compare_symbol_tables_i(&map1->values, &map2->values TSRMLS_CC) == 0)
    return 0;

  return 1;
}

static void
php_cassandra_map_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_map *self = PHP5TO7_ZEND_OBJECT_GET(map, object);

  zend_hash_destroy(&self->keys);
  zend_hash_destroy(&self->values);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_map_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_map *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(map, ce);

  zend_hash_init(&self->keys, 0, NULL, ZVAL_PTR_DTOR, 0);
  zend_hash_init(&self->values, 0, NULL, ZVAL_PTR_DTOR, 0);

  PHP5TO7_ZEND_OBJECT_INIT(map, self, ce);
}

void cassandra_define_Map(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Map", cassandra_map_methods);
  cassandra_map_ce = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_map_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_map_handlers.get_properties  = php_cassandra_map_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_map_handlers.get_gc          = php_cassandra_map_gc;
#endif
  cassandra_map_handlers.compare_objects = php_cassandra_map_compare;
  cassandra_map_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_map_ce->create_object = php_cassandra_map_new;
  zend_class_implements(cassandra_map_ce TSRMLS_CC, 3, spl_ce_Countable, zend_ce_iterator, zend_ce_arrayaccess);
}
