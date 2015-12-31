#include "php_cassandra.h"
#include "util/collections.h"
#include "src/Cassandra/Set.h"

zend_class_entry *cassandra_set_ce = NULL;

int
php_cassandra_set_add(cassandra_set *set, zval *object TSRMLS_DC)
{
  char *key;
  int   key_len;
  int   result = 0;

  if (Z_TYPE_P(object) == IS_NULL) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid value: null is not supported inside sets");
    return 0;
  }

  if (!php_cassandra_hash_object(object, set->type, &key, &key_len TSRMLS_CC))
    return 0;

  if (PHP5TO7_ZEND_HASH_ADD(&set->values, key, key_len + 1, object, sizeof(zval*))) {
    Z_TRY_ADDREF_P(object);
    result = 1;
  }

  efree(key);
  return result;
}

static int
php_cassandra_set_del(cassandra_set *set, zval *object TSRMLS_DC)
{
  char *key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(object, set->type, &key, &key_len TSRMLS_CC))
    return 0;

  if (PHP5TO7_ZEND_HASH_DEL(&set->values, key, key_len + 1))
    result = 1;

  efree(key);
  return result;
}

static int
php_cassandra_set_has(cassandra_set *set, zval *object TSRMLS_DC)
{
  char *key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(object, set->type, &key, &key_len TSRMLS_CC))
    return 0;

  result = PHP5TO7_ZEND_HASH_EXISTS(&set->values, key, key_len + 1);

  efree(key);
  return result;
}

static void
php_cassandra_set_populate(cassandra_set *set, zval *array)
{
  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_VAL(&set->values, current) {
    if (add_next_index_zval(array, PHP5TO7_ZVAL_MAYBE_DEREF(current)) == SUCCESS)
      Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_DEREF(current));
    else
      break;
  } PHP5TO7_ZEND_HASH_FOREACH_END(&set->values);
}

/* {{{ Cassandra\Set::__construct(string) */
PHP_METHOD(Set, __construct)
{
  char *type;
  php5to7_size type_len;
  cassandra_set *set;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &type, &type_len) == FAILURE) {
    return;
  }

  set = PHP_CASSANDRA_GET_SET(getThis());

  php_cassandra_value_type(type, &set->type TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Set::type() */
PHP_METHOD(Set, type)
{
  cassandra_set *set = PHP_CASSANDRA_GET_SET(getThis());

  PHP5TO7_RETVAL_STRING(php_cassandra_type_name(set->type));
}
/* }}} */

/* {{{ Cassandra\Set::values() */
PHP_METHOD(Set, values)
{
  cassandra_set *set = NULL;
  array_init(return_value);
  set = PHP_CASSANDRA_GET_SET(getThis());
  php_cassandra_set_populate(set, return_value);
}
/* }}} */

/* {{{ Cassandra\Set::add(value) */
PHP_METHOD(Set, add)
{
  cassandra_set *set = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  set = PHP_CASSANDRA_GET_SET(getThis());

  if (php_cassandra_set_add(set, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::remove(value) */
PHP_METHOD(Set, remove)
{
  cassandra_set *set = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  set = PHP_CASSANDRA_GET_SET(getThis());

  if (php_cassandra_set_del(set, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::has(value) */
PHP_METHOD(Set, has)
{
  cassandra_set *set = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  set = PHP_CASSANDRA_GET_SET(getThis());

  if (php_cassandra_set_has(set, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::count() */
PHP_METHOD(Set, count)
{
  cassandra_set *set = PHP_CASSANDRA_GET_SET(getThis());
  RETURN_LONG(zend_hash_num_elements(&set->values));
}
/* }}} */

/* {{{ Cassandra\Set::current() */
PHP_METHOD(Set, current)
{
  php5to7_zval *current;
  cassandra_set *set = PHP_CASSANDRA_GET_SET(getThis());

  if (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(&set->values, current))
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(current), 1, 0);
}
/* }}} */

/* {{{ Cassandra\Set::key() */
PHP_METHOD(Set, key)
{
  cassandra_set *set = PHP_CASSANDRA_GET_SET(getThis());
  RETURN_LONG(set->pos);
}
/* }}} */

/* {{{ Cassandra\Set::next() */
PHP_METHOD(Set, next)
{
  cassandra_set *set = PHP_CASSANDRA_GET_SET(getThis());
  if (zend_hash_move_forward(&set->values) == SUCCESS)
    set->pos++;
}
/* }}} */

/* {{{ Cassandra\Set::valid() */
PHP_METHOD(Set, valid)
{
  cassandra_set *set = PHP_CASSANDRA_GET_SET(getThis());
  RETURN_BOOL(zend_hash_has_more_elements(&set->values) == SUCCESS);
}
/* }}} */

/* {{{ Cassandra\Set::rewind() */
PHP_METHOD(Set, rewind)
{
  cassandra_set *set = PHP_CASSANDRA_GET_SET(getThis());
  zend_hash_internal_pointer_reset(&set->values);
  set->pos = 0;
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_one, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_set_methods[] = {
  PHP_ME(Set, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Set, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, add, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Set, remove, arginfo_one, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(Set, count, arginfo_none, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(Set, current, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, key, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, next, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, valid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, rewind, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_set_handlers;

static HashTable*
php_cassandra_set_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_set_properties(zval *object TSRMLS_DC)
{
  cassandra_set *self = PHP_CASSANDRA_GET_SET(object);
  HashTable     *props = zend_std_get_properties(object TSRMLS_CC);
  php5to7_zval   values;


  PHP5TO7_ZVAL_MAYBE_MAKE(values);
  array_init(PHP5TO7_ZVAL_MAYBE_P(values));

  php_cassandra_set_populate(self , PHP5TO7_ZVAL_MAYBE_P(values));

  PHP5TO7_ZEND_HASH_UPDATE(props, "values", sizeof("values"), PHP5TO7_ZVAL_MAYBE_P(values), sizeof(zval));

  return props;
}

int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC);

static int
php_cassandra_set_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_set *set1 = NULL;
  cassandra_set *set2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  set1 = PHP_CASSANDRA_GET_SET(obj1);
  set2 = PHP_CASSANDRA_GET_SET(obj2);

  if (set1->type != set2->type)
    return 1;

  return zend_compare_symbol_tables_i(&set1->values, &set2->values TSRMLS_CC);
}

static void
php_cassandra_set_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_set *self = PHP5TO7_ZEND_OBJECT_GET(set, object);

  zend_hash_destroy(&self->values);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_set_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_set *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(set, ce);

  zend_hash_init(&self->values, 0, NULL, ZVAL_PTR_DTOR, 0);

  PHP5TO7_ZEND_OBJECT_INIT(set, self, ce);
}

void cassandra_define_Set(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Set", cassandra_set_methods);
  cassandra_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_set_handlers.get_properties  = php_cassandra_set_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_set_handlers.get_gc          = php_cassandra_set_gc;
#endif
  cassandra_set_handlers.compare_objects = php_cassandra_set_compare;
  cassandra_set_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_set_ce->create_object = php_cassandra_set_new;
  zend_class_implements(cassandra_set_ce TSRMLS_CC, 2, spl_ce_Countable, zend_ce_iterator);
}
