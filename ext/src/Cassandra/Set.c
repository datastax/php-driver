#include "php_cassandra.h"
#include "util/collections.h"
#include "util/hash.h"
#include "util/types.h"
#include "src/Cassandra/Set.h"

zend_class_entry *cassandra_set_ce = NULL;

int
php_cassandra_set_add(cassandra_set *set, zval *object TSRMLS_DC)
{
  cassandra_set_entry *entry;
  cassandra_type *type;

  if (Z_TYPE_P(object) == IS_NULL) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid key: null is not supported inside sets");
    return 0;
  }

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));

  if (!php_cassandra_validate_object(object, PHP5TO7_ZVAL_MAYBE_P(type->value_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(set->entries, object, entry);
  if (entry == NULL) {
    set->dirty = 1;
    entry = (cassandra_set_entry *) emalloc(sizeof(cassandra_set_entry));
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(entry->value), object);
    HASH_ADD_ZVAL(set->entries, value, entry);
  }

  return 1;
}

static int
php_cassandra_set_del(cassandra_set *set, zval *object TSRMLS_DC)
{
  cassandra_set_entry *entry;
  cassandra_type *type;
  int result = 0;

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));

  if (!php_cassandra_validate_object(object, PHP5TO7_ZVAL_MAYBE_P(type->value_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(set->entries, object, entry);
  if (entry != NULL) {
    set->dirty = 1;
    if (entry == set->iter_temp) {
      set->iter_temp = (cassandra_set_entry *)set->iter_temp->hh.next;
    }
    HASH_DEL(set->entries, entry);
    zval_ptr_dtor(&entry->value);
    efree(entry);
    result = 1;
  }

  return result;
}

static int
php_cassandra_set_has(cassandra_set *set, zval *object TSRMLS_DC)
{
  cassandra_set_entry *entry;
  cassandra_type *type;
  int result = 0;

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));

  if (!php_cassandra_validate_object(object, PHP5TO7_ZVAL_MAYBE_P(type->value_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(set->entries, object, entry);
  if (entry != NULL) {
    result = 1;
  }

  return result;
}

static void
php_cassandra_set_populate(cassandra_set *set, zval *array)
{
  cassandra_set_entry *curr, *temp;
  HASH_ITER(hh, set->entries, curr, temp) {
    if (add_next_index_zval(array, PHP5TO7_ZVAL_MAYBE_P(curr->value)) != SUCCESS) {
      break;
    }
    Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(curr->value));
  }
}

/* {{{ Cassandra\Set::__construct(type) */
PHP_METHOD(Set, __construct)
{
  cassandra_set *self;
  zval *value_type;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value_type) == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_SET(getThis());

  if (Z_TYPE_P(value_type) == IS_STRING) {
    CassValueType type;
    if (!php_cassandra_value_type(Z_STRVAL_P(value_type), &type TSRMLS_CC))
      return;
    self->type = php_cassandra_type_set_from_value_type(type TSRMLS_CC);
  } else if (Z_TYPE_P(value_type) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(value_type), cassandra_type_ce TSRMLS_CC)) {
    self->type = php_cassandra_type_set(value_type TSRMLS_CC);
    Z_ADDREF_P(value_type);
  } else {
    INVALID_ARGUMENT(value_type, "a string or an instance of Cassandra\\Type");
  }
}
/* }}} */

/* {{{ Cassandra\Set::type() */
PHP_METHOD(Set, type)
{
  cassandra_set *self = PHP_CASSANDRA_GET_SET(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->type), 1, 0);
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
  cassandra_set *self = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_SET(getThis());

  if (php_cassandra_set_add(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::remove(value) */
PHP_METHOD(Set, remove)
{
  cassandra_set *self = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_SET(getThis());

  if (php_cassandra_set_del(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::has(value) */
PHP_METHOD(Set, has)
{
  cassandra_set *self = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_SET(getThis());

  if (php_cassandra_set_has(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::count() */
PHP_METHOD(Set, count)
{
  cassandra_set *self = PHP_CASSANDRA_GET_SET(getThis());
  RETURN_LONG((long)HASH_COUNT(self->entries));
}
/* }}} */

/* {{{ Cassandra\Set::current() */
PHP_METHOD(Set, current)
{
  cassandra_set *self = PHP_CASSANDRA_GET_SET(getThis());
  if (self->iter_curr != NULL)
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->iter_curr->value), 1, 0);
}
/* }}} */

/* {{{ Cassandra\Set::key() */
PHP_METHOD(Set, key)
{
  cassandra_set *self = PHP_CASSANDRA_GET_SET(getThis());
  RETURN_LONG(self->iter_index);
}
/* }}} */

/* {{{ Cassandra\Set::next() */
PHP_METHOD(Set, next)
{
  cassandra_set *self = PHP_CASSANDRA_GET_SET(getThis());
  self->iter_curr = self->iter_temp;
  self->iter_temp = self->iter_temp != NULL ? (cassandra_set_entry *)self->iter_temp->hh.next : NULL;
  self->iter_index++;
}
/* }}} */

/* {{{ Cassandra\Set::valid() */
PHP_METHOD(Set, valid)
{
  cassandra_set *self = PHP_CASSANDRA_GET_SET(getThis());
  RETURN_BOOL(self->iter_curr != NULL);
}
/* }}} */

/* {{{ Cassandra\Set::rewind() */
PHP_METHOD(Set, rewind)
{
  cassandra_set *self = PHP_CASSANDRA_GET_SET(getThis());
  self->iter_curr = self->entries;
  self->iter_temp = self->entries != NULL ? (cassandra_set_entry *)self->entries->hh.next : NULL;
  self->iter_index = 0;
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
  PHP_ME(Set, has, arginfo_one, ZEND_ACC_PUBLIC)
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

static php_cassandra_value_handlers cassandra_set_handlers;

static HashTable *
php_cassandra_set_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
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
  cassandra_set_entry *iter1;
  cassandra_set_entry *iter2;
  cassandra_set *set1;
  cassandra_set *set2;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  set1 = PHP_CASSANDRA_GET_SET(obj1);
  set2 = PHP_CASSANDRA_GET_SET(obj2);

  if (HASH_COUNT(set1->entries) != HASH_COUNT(set1->entries)) {
   return HASH_COUNT(set1->entries) < HASH_COUNT(set1->entries) ? -1 : 1;
  }

  iter1 = set1->entries;
  iter2 = set2->entries;
  while (iter1 && iter2) {
    int r = php_cassandra_value_compare(PHP5TO7_ZVAL_MAYBE_P(iter1->value),
                                        PHP5TO7_ZVAL_MAYBE_P(iter2->value) TSRMLS_CC);
    if (r != 0) return r;
    iter1 = (cassandra_set_entry *) iter1->hh.next;
    iter2 = (cassandra_set_entry *) iter2->hh.next;
  }

  return 0;
}

static unsigned
php_cassandra_set_hash_value(zval *obj TSRMLS_DC)
{
  unsigned hashv = 0;
  cassandra_set_entry *curr,  *temp;
  cassandra_set *self = PHP_CASSANDRA_GET_SET(obj);

  if (!self->dirty) return self->hashv;

  HASH_ITER(hh, self->entries, curr, temp) {
    hashv = php_cassandra_combine_hash(hashv, php_cassandra_value_hash(PHP5TO7_ZVAL_MAYBE_P(curr->value) TSRMLS_CC));
  }

  self->hashv = hashv;
  self->dirty = 0;

  return hashv;
}

static void
php_cassandra_set_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_set *self = PHP5TO7_ZEND_OBJECT_GET(set, object);
  cassandra_set_entry *curr, *temp;

  HASH_ITER(hh, self->entries, curr, temp) {
    zval_ptr_dtor(&curr->value);
    HASH_DEL(self->entries, curr);
    efree(curr);
  }

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->type);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_set_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_set *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(set, ce);

  self->entries = self->iter_curr = self->iter_temp = NULL;
  self->iter_index = 0;
  self->dirty = 1;
  PHP5TO7_ZVAL_UNDEF(self->type);

  PHP5TO7_ZEND_OBJECT_INIT(set, self, ce);
}

void cassandra_define_Set(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Set", cassandra_set_methods);
  cassandra_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_set_ce TSRMLS_CC, 1, cassandra_value_ce);
  memcpy(&cassandra_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_set_handlers.std.get_properties  = php_cassandra_set_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_set_handlers.std.get_gc          = php_cassandra_set_gc;
#endif
  cassandra_set_handlers.std.compare_objects = php_cassandra_set_compare;
  cassandra_set_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_set_ce->create_object = php_cassandra_set_new;
  zend_class_implements(cassandra_set_ce TSRMLS_CC, 2, spl_ce_Countable, zend_ce_iterator);

  cassandra_set_handlers.hash_value = php_cassandra_set_hash_value;
}
