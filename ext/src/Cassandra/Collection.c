#include "php_cassandra.h"
#include "util/collections.h"
#include "src/Cassandra/Collection.h"

zend_class_entry *cassandra_collection_ce = NULL;

int
php_cassandra_collection_add(cassandra_collection* collection, zval* object TSRMLS_DC)
{
  // TODO(mpenick)
#if 0
  if (zend_hash_next_index_insert(&collection->values, (void*) &object, sizeof(zval*), NULL) == SUCCESS) {
    Z_ADDREF_P(object);
    return 1;
  }
#endif

  return 0;
}

static int
php_cassandra_collection_del(cassandra_collection* collection, ulong index)
{
  if (zend_hash_index_del(&collection->values, index) == SUCCESS)
    return 1;

  return 0;
}

static int
php_cassandra_collection_get(cassandra_collection* collection, ulong index, zval** zvalue)
{
  // TODO(mpenick)
#if 0
  zval** value;

  if (zend_hash_index_find(&collection->values, index, (void**) &value) == SUCCESS) {
    *zvalue = *value;
    return 1;
  }
#endif

  return 0;
}

static int
php_cassandra_collection_find(cassandra_collection* collection, zval* object, long* index TSRMLS_DC)
{
  // TODO(mpenick)
#if 0
  HashPointer ptr;
  zval**      current;
  zval        compare;
  ulong       idx;

  if (!php_cassandra_validate_object(object, collection->type TSRMLS_CC))
    return 0;

  zend_hash_get_pointer(&collection->values, &ptr);
  zend_hash_internal_pointer_reset(&collection->values);

  while (zend_hash_get_current_data(&collection->values, (void**) &current) == SUCCESS) {
    is_equal_function(&compare, object, *current TSRMLS_CC);
    if (Z_LVAL(compare) && zend_hash_get_current_key(&collection->values, NULL, &idx, 0) == HASH_KEY_IS_LONG) {
      *index = (long) idx;
      return 1;
    }

    zend_hash_move_forward(&collection->values);
  }


  zend_hash_set_pointer(&collection->values, &ptr);
#endif

  return 0;
}

static void
php_cassandra_collection_populate(cassandra_collection* collection, zval* array)
{
  // TODO(mpenick)
#if 0
  HashPointer ptr;
  zval** current;

  zend_hash_get_pointer(&collection->values, &ptr);
  zend_hash_internal_pointer_reset(&collection->values);

  while (zend_hash_get_current_data(&collection->values, (void**) &current) == SUCCESS) {
    if (add_next_index_zval(array, *current) == SUCCESS)
      Z_ADDREF_PP(current);
    else
      break;

    zend_hash_move_forward(&collection->values);
  }

  zend_hash_set_pointer(&collection->values, &ptr);
#endif
}

/* {{{ Cassandra\Collection::__construct(string) */
PHP_METHOD(Collection, __construct)
{
  char *type;
  int type_len;
  cassandra_collection* collection;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &type, &type_len) == FAILURE) {
    return;
  }

  collection = PHP_CASSANDRA_GET_COLLECTION(getThis());

  php_cassandra_value_type(type, &collection->type TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Collection::type() */
PHP_METHOD(Collection, type)
{
  cassandra_collection* collection = PHP_CASSANDRA_GET_COLLECTION(getThis());

  PHP5TO7_RETURN_STRING(php_cassandra_type_name(collection->type));
}
/* }}} */

/* {{{ Cassandra\Collection::values() */
PHP_METHOD(Collection, values)
{
  cassandra_collection* collection = NULL;
  array_init(return_value);
  collection = PHP_CASSANDRA_GET_COLLECTION(getThis());
  php_cassandra_collection_populate(collection, return_value);
}
/* }}} */

/* {{{ Cassandra\Collection::add(mixed) */
PHP_METHOD(Collection, add)
{
  zval*** args;
  cassandra_collection* collection = NULL;
  int argc, i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE)
    return;

  collection = PHP_CASSANDRA_GET_COLLECTION(getThis());

  for (i = 0; i < argc; i++) {
    if (Z_TYPE_P(*args[i]) == IS_NULL) {
      efree(args);
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "Invalid value: null is not supported inside collections");
      RETURN_FALSE;
    }

    if (!php_cassandra_validate_object(*args[i], collection->type TSRMLS_CC)) {
      efree(args);
      RETURN_FALSE;
    }
  }

  for (i = 0; i < argc; i++) {
    php_cassandra_collection_add(collection, *args[i] TSRMLS_CC);
  }

  efree(args);
  RETVAL_LONG(zend_hash_num_elements(&collection->values));
}
/* }}} */

/* {{{ Cassandra\Collection::get(int) */
PHP_METHOD(Collection, get)
{
  long key;
  cassandra_collection* collection = NULL;
  zval* value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &key) == FAILURE)
    return;

  collection = PHP_CASSANDRA_GET_COLLECTION(getThis());

  if (php_cassandra_collection_get(collection, (ulong) key, &value))
    RETURN_ZVAL(value, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Collection::find(mixed) */
PHP_METHOD(Collection, find)
{
  zval* object;
  cassandra_collection* collection = NULL;
  long index;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  collection = PHP_CASSANDRA_GET_COLLECTION(getThis());

  if (php_cassandra_collection_find(collection, object, &index TSRMLS_CC))
    RETURN_LONG(index);
}
/* }}} */

/* {{{ Cassandra\Collection::count() */
PHP_METHOD(Collection, count)
{
  cassandra_collection* collection = PHP_CASSANDRA_GET_COLLECTION(getThis());
  RETURN_LONG(zend_hash_num_elements(&collection->values));
}
/* }}} */

/* {{{ Cassandra\Collection::current() */
PHP_METHOD(Collection, current)
{
  // TODO(mpenick)
#if 0
  zval** current;
  cassandra_collection* collection = PHP_CASSANDRA_GET_COLLECTION(getThis());

  if (zend_hash_get_current_data(&collection->values, (void**) &current) == SUCCESS)
    RETURN_ZVAL(*current, 1, 0);
#endif
}
/* }}} */

/* {{{ Cassandra\Collection::key() */
PHP_METHOD(Collection, key)
{
  // TODO(mpenick)
#if 0
  ulong index;
  cassandra_collection* collection = PHP_CASSANDRA_GET_COLLECTION(getThis());

  if (zend_hash_get_current_key(&collection->values, NULL, &index, 0) == HASH_KEY_IS_LONG)
    RETURN_LONG(index);
#endif
}
/* }}} */

/* {{{ Cassandra\Collection::next() */
PHP_METHOD(Collection, next)
{
  cassandra_collection* collection = PHP_CASSANDRA_GET_COLLECTION(getThis());
  zend_hash_move_forward(&collection->values);
}
/* }}} */

/* {{{ Cassandra\Collection::valid() */
PHP_METHOD(Collection, valid)
{
  cassandra_collection* collection = PHP_CASSANDRA_GET_COLLECTION(getThis());
  RETURN_BOOL(zend_hash_has_more_elements(&collection->values) == SUCCESS);
}
/* }}} */

/* {{{ Cassandra\Collection::rewind() */
PHP_METHOD(Collection, rewind)
{
  cassandra_collection* collection = PHP_CASSANDRA_GET_COLLECTION(getThis());
  zend_hash_internal_pointer_reset(&collection->values);
}
/* }}} */

/* {{{ Cassandra\Collection::remove(key) */
PHP_METHOD(Collection, remove)
{
  long index;
  cassandra_collection* collection = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
    return;
  }

  collection = PHP_CASSANDRA_GET_COLLECTION(getThis());

  if (php_cassandra_collection_del(collection, (ulong) index)) {
    RETURN_TRUE;
  }

  RETURN_FALSE;
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_index, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_collection_methods[] = {
  PHP_ME(Collection, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Collection, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, add, arginfo_value, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, get, arginfo_index, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, find, arginfo_value, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(Collection, count, arginfo_none, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(Collection, current, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, key, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, next, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, valid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, rewind, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Collection, remove, arginfo_index, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_collection_handlers;

static HashTable*
php_cassandra_collection_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_collection_properties(zval *object TSRMLS_DC)
{
  cassandra_collection  *collection = PHP_CASSANDRA_GET_COLLECTION(object);
  HashTable             *props      = zend_std_get_properties(object TSRMLS_CC);
  zval                  *values     = NULL;


  PHP5TO7_ZVAL_MAYBE_MAKE(values);
  array_init(values);

  php_cassandra_collection_populate(collection, values);

  PHP5TO7_ZEND_HASH_UPDATE(props, "values", sizeof("values"), values, sizeof(zval));

  return props;
}

int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC);

static int
php_cassandra_collection_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_collection* collection1 = NULL;
  cassandra_collection* collection2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  collection1 = PHP_CASSANDRA_GET_COLLECTION(obj1);
  collection2 = PHP_CASSANDRA_GET_COLLECTION(obj2);

  if (collection1->type != collection2->type)
    return 1;

  return zend_compare_symbol_tables_i(&collection1->values, &collection2->values TSRMLS_CC);
}

static void
php_cassandra_collection_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_collection *self = (cassandra_collection *) object;

  zend_hash_destroy(&self->values);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_ZEND_OBJECT_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_collection_new(zend_class_entry* ce TSRMLS_DC)
{
  cassandra_collection *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(collection, ce);

  zend_hash_init(&self->values, 0, NULL, ZVAL_PTR_DTOR, 0);

  PHP5TO7_ZEND_OBJECT_INIT(collection, self, ce);
}

void cassandra_define_Collection(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Collection", cassandra_collection_methods);
  cassandra_collection_ce = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_collection_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_collection_handlers.get_properties  = php_cassandra_collection_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_collection_handlers.get_gc          = php_cassandra_collection_gc;
#endif
  cassandra_collection_handlers.compare_objects = php_cassandra_collection_compare;
  cassandra_collection_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_collection_ce->create_object = php_cassandra_collection_new;
  zend_class_implements(cassandra_collection_ce TSRMLS_CC, 2, spl_ce_Countable, zend_ce_iterator);
}
