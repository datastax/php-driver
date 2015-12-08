#include "php_cassandra.h"
#include "util/collections.h"
#include "util/hash.h"
#include "util/types.h"
#include "src/Cassandra/Set.h"

zend_class_entry *cassandra_set_ce = NULL;

int
php_cassandra_set_add(cassandra_set* set, zval* object TSRMLS_DC)
{
  cassandra_set_entry* entry;
  cassandra_type_set* type;

  if (Z_TYPE_P(object) == IS_NULL) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid key: null is not supported inside sets");
    return 0;
  }

  type = (cassandra_type_set*) zend_object_store_get_object(set->type TSRMLS_CC);

  if (!php_cassandra_validate_object(object, type->value_type TSRMLS_CC)) {
    return 0;
  }

  set->dirty = 1;
  HASH_FIND_PTR(set->entries, &object, entry);
  if (entry == NULL) {
    entry = (cassandra_set_entry*)emalloc(sizeof(cassandra_set_entry));
    entry->value = object;
    Z_ADDREF_P(entry->value);
    HASH_ADD_PTR(set->entries, value, entry);
  } else if (entry->value != object) {
    entry->value = object;
    Z_ADDREF_P(entry->value);
  }

  return 1;
}

static int
php_cassandra_set_del(cassandra_set* set, zval* object TSRMLS_DC)
{
  cassandra_set_entry* entry;
  cassandra_type_set* type;
  int result = 0;

  type = (cassandra_type_set*) zend_object_store_get_object(set->type TSRMLS_CC);

  if (!php_cassandra_validate_object(object, type->value_type TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_PTR(set->entries, &object, entry);
  if (entry != NULL) {
    set->dirty = 1;
    if (entry == set->iter_temp) {
      set->iter_temp = (cassandra_set_entry*)set->iter_temp->hh.next;
    }
    HASH_DEL(set->entries, entry);
    zval_ptr_dtor(&entry->value);
    efree(entry);
    result = 1;
  }

  return result;
}

static int
php_cassandra_set_has(cassandra_set* set, zval* object TSRMLS_DC)
{
  cassandra_set_entry* entry;
  cassandra_type_set* type;
  int result = 0;

  type = (cassandra_type_set*) zend_object_store_get_object(set->type TSRMLS_CC);

  if (!php_cassandra_validate_object(object, type->value_type TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_PTR(set->entries, &object, entry);
  if (entry != NULL) {
    result = 1;
  }

  return result;
}

static void
php_cassandra_set_populate(cassandra_set* set, zval* array)
{
  cassandra_set_entry* curr, * temp;
  HASH_ITER(hh, set->entries, curr, temp) {
    if (add_next_index_zval(array, curr->value) != SUCCESS) {
      break;
    }
    Z_ADDREF_P(curr->value);
  }
}

/* {{{ Cassandra\Set::__construct(string) */
PHP_METHOD(Set, __construct)
{
  cassandra_set* self;
  zval* value_type;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O",
                            &value_type, cassandra_type_ce) == FAILURE) {
    return;
  }

  self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  self->type = php_cassandra_type_set(value_type TSRMLS_CC);
  Z_ADDREF_P(value_type);
}
/* }}} */

/* {{{ Cassandra\Set::type() */
PHP_METHOD(Set, type)
{
  cassandra_map* self = (cassandra_map*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->type, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Set::valueType() */
PHP_METHOD(Set, valueType)
{
  cassandra_set* self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  cassandra_type_set* type = (cassandra_type_set*) zend_object_store_get_object(self->type TSRMLS_CC);
  RETURN_ZVAL(type->value_type, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Set::values() */
PHP_METHOD(Set, values)
{
  cassandra_set* self = NULL;
  array_init(return_value);
  self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  php_cassandra_set_populate(self, return_value);
}
/* }}} */

/* {{{ Cassandra\Set::add(value) */
PHP_METHOD(Set, add)
{
  cassandra_set* self = NULL;

  zval* object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_set_add(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::remove(value) */
PHP_METHOD(Set, remove)
{
  cassandra_set* self = NULL;

  zval* object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_set_del(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::has(value) */
PHP_METHOD(Set, has)
{
  cassandra_set* self = NULL;

  zval* object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_set_has(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::count() */
PHP_METHOD(Set, count)
{
  cassandra_set* self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG((long)HASH_COUNT(self->entries));
}
/* }}} */

/* {{{ Cassandra\Set::current() */
PHP_METHOD(Set, current)
{
  cassandra_set* self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  if (self->iter_curr != NULL)
    RETURN_ZVAL(self->iter_curr->value, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Set::key() */
PHP_METHOD(Set, key)
{
  cassandra_set* self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(self->iter_index);
}
/* }}} */

/* {{{ Cassandra\Set::next() */
PHP_METHOD(Set, next)
{
  cassandra_set* self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  self->iter_curr = self->iter_temp;
  self->iter_temp = self->iter_temp != NULL ? (cassandra_set_entry*)self->iter_temp->hh.next : NULL;
  self->iter_index++;
}
/* }}} */

/* {{{ Cassandra\Set::valid() */
PHP_METHOD(Set, valid)
{
  cassandra_set* self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(self->iter_curr != NULL);
}
/* }}} */

/* {{{ Cassandra\Set::rewind() */
PHP_METHOD(Set, rewind)
{
  cassandra_set* self = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  self->iter_curr = self->entries;
  self->iter_temp = self->entries != NULL ? (cassandra_set_entry*)self->entries->hh.next : NULL;
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
  PHP_ME(Set, valueType, arginfo_none, ZEND_ACC_PUBLIC)
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
php_cassandra_set_gc(zval *object, zval ***table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_set_properties(zval *object TSRMLS_DC)
{
  zval* values;

  cassandra_set* self  = (cassandra_set*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*     props = zend_std_get_properties(object TSRMLS_CC);

  MAKE_STD_ZVAL(values);
  array_init(values);

  php_cassandra_set_populate(self, values);

  zend_hash_update(props, "values", sizeof("values"), &values, sizeof(zval), NULL);

  return props;
}

int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC);

static int
php_cassandra_set_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return php_cassandra_value_compare(obj1, obj2 TSRMLS_CC);
}

static void
php_cassandra_set_free(void *object TSRMLS_DC)
{
  cassandra_set* self = (cassandra_set*) object;
  cassandra_set_entry* curr, * temp;

  zval_ptr_dtor(&self->type); /* TODO(mpenick): Move to Value dtor? */

  HASH_ITER(hh, self->entries, curr, temp) {
    zval_ptr_dtor(&curr->value);
    HASH_DEL(self->entries, curr);
    efree(curr);
  }

  zval_ptr_dtor(&self->type);
  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_set_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_set *set;

  set = (cassandra_set*) emalloc(sizeof(cassandra_set));
  memset(set, 0, sizeof(cassandra_set));

  set->type = NULL;
  set->entries = set->iter_curr = set->iter_temp = NULL;
  set->iter_index = 0;
  set->dirty = 1;

  zend_object_std_init(&set->zval, class_type TSRMLS_CC);
  object_properties_init(&set->zval, class_type);

  retval.handle   = zend_objects_store_put(set,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_set_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_set_handlers;

  return retval;
}

void cassandra_define_Set(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Set", cassandra_set_methods);
  cassandra_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_set_ce TSRMLS_CC, 1, cassandra_value_ce);
  memcpy(&cassandra_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_set_handlers.get_properties  = php_cassandra_set_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_set_handlers.get_gc          = php_cassandra_set_gc;
#endif
  cassandra_set_handlers.compare_objects = php_cassandra_set_compare;
  cassandra_set_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_set_ce->create_object = php_cassandra_set_new;
  zend_class_implements(cassandra_set_ce TSRMLS_CC, 2, spl_ce_Countable, zend_ce_iterator);
}
