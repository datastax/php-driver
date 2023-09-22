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
#include "src/Set.h"

zend_class_entry *php_driver_set_ce = NULL;

int
php_driver_set_add(php_driver_set *set, zval *object TSRMLS_DC)
{
  php_driver_set_entry *entry;
  php_driver_type *type;

  if (Z_TYPE_P(object) == IS_NULL) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid value: null is not supported inside sets");
    return 0;
  }

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));

  if (!php_driver_validate_object(object, PHP5TO7_ZVAL_MAYBE_P(type->data.set.value_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(set->entries, object, entry);
  if (entry == NULL) {
    set->dirty = 1;
    entry = (php_driver_set_entry *) emalloc(sizeof(php_driver_set_entry));
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(entry->value), object);
    HASH_ADD_ZVAL(set->entries, value, entry);
  }

  return 1;
}

static int
php_driver_set_del(php_driver_set *set, zval *object TSRMLS_DC)
{
  php_driver_set_entry *entry;
  php_driver_type *type;
  int result = 0;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));

  if (!php_driver_validate_object(object, PHP5TO7_ZVAL_MAYBE_P(type->data.set.value_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(set->entries, object, entry);
  if (entry != NULL) {
    set->dirty = 1;
    if (entry == set->iter_temp) {
      set->iter_temp = (php_driver_set_entry *)set->iter_temp->hh.next;
    }
    HASH_DEL(set->entries, entry);
    zval_ptr_dtor(&entry->value);
    efree(entry);
    result = 1;
  }

  return result;
}

static int
php_driver_set_has(php_driver_set *set, zval *object TSRMLS_DC)
{
  php_driver_set_entry *entry;
  php_driver_type *type;
  int result = 0;

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));

  if (!php_driver_validate_object(object, PHP5TO7_ZVAL_MAYBE_P(type->data.set.value_type) TSRMLS_CC)) {
    return 0;
  }

  HASH_FIND_ZVAL(set->entries, object, entry);
  if (entry != NULL) {
    result = 1;
  }

  return result;
}

static void
php_driver_set_populate(php_driver_set *set, zval *array TSRMLS_DC)
{
  php_driver_set_entry *curr, *temp;
  HASH_ITER(hh, set->entries, curr, temp) {
    if (add_next_index_zval(array, PHP5TO7_ZVAL_MAYBE_P(curr->value)) != SUCCESS) {
      break;
    }
    Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(curr->value));
  }
}

/* {{{ Set::__construct(type) */
PHP_METHOD(Set, __construct)
{
  php_driver_set *self;
  zval *type;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type) == FAILURE)
    return;

  self = PHP_DRIVER_GET_SET(getThis());

  if (Z_TYPE_P(type) == IS_STRING) {
    CassValueType value_type;
    if (!php_driver_value_type(Z_STRVAL_P(type), &value_type TSRMLS_CC))
      return;
    self->type = php_driver_type_set_from_value_type(value_type TSRMLS_CC);
  } else if (Z_TYPE_P(type) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(type), php_driver_type_ce TSRMLS_CC)) {
    if (!php_driver_type_validate(type, "type" TSRMLS_CC)) {
      return;
    }
    self->type = php_driver_type_set(type TSRMLS_CC);
    Z_ADDREF_P(type);
  } else {
    INVALID_ARGUMENT(type, "a string or an instance of " PHP_DRIVER_NAMESPACE "\\Type");
  }
}
/* }}} */

/* {{{ Set::type() */
PHP_METHOD(Set, type)
{
  php_driver_set *self = PHP_DRIVER_GET_SET(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->type), 1, 0);
}
/* }}} */

/* {{{ Set::values() */
PHP_METHOD(Set, values)
{
  php_driver_set *set = NULL;
  array_init(return_value);
  set = PHP_DRIVER_GET_SET(getThis());
  php_driver_set_populate(set, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ Set::add(value) */
PHP_METHOD(Set, add)
{
  php_driver_set *self = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = PHP_DRIVER_GET_SET(getThis());

  if (php_driver_set_add(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Set::remove(value) */
PHP_METHOD(Set, remove)
{
  php_driver_set *self = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = PHP_DRIVER_GET_SET(getThis());

  if (php_driver_set_del(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Set::has(value) */
PHP_METHOD(Set, has)
{
  php_driver_set *self = NULL;

  zval *object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  self = PHP_DRIVER_GET_SET(getThis());

  if (php_driver_set_has(self, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Set::count() */
PHP_METHOD(Set, count)
{
  php_driver_set *self = PHP_DRIVER_GET_SET(getThis());
  RETURN_LONG((long)HASH_COUNT(self->entries));
}
/* }}} */

/* {{{ Set::current() */
PHP_METHOD(Set, current)
{
  php_driver_set *self = PHP_DRIVER_GET_SET(getThis());
  if (self->iter_curr != NULL)
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->iter_curr->value), 1, 0);
}
/* }}} */

/* {{{ Set::key() */
PHP_METHOD(Set, key)
{
  php_driver_set *self = PHP_DRIVER_GET_SET(getThis());
  RETURN_LONG(self->iter_index);
}
/* }}} */

/* {{{ Set::next() */
PHP_METHOD(Set, next)
{
  php_driver_set *self = PHP_DRIVER_GET_SET(getThis());
  self->iter_curr = self->iter_temp;
  self->iter_temp = self->iter_temp != NULL ? (php_driver_set_entry *)self->iter_temp->hh.next : NULL;
  self->iter_index++;
}
/* }}} */

/* {{{ Set::valid() */
PHP_METHOD(Set, valid)
{
  php_driver_set *self = PHP_DRIVER_GET_SET(getThis());
  RETURN_BOOL(self->iter_curr != NULL);
}
/* }}} */

/* {{{ Set::rewind() */
PHP_METHOD(Set, rewind)
{
  php_driver_set *self = PHP_DRIVER_GET_SET(getThis());
  self->iter_curr = self->entries;
  self->iter_temp = self->entries != NULL ? (php_driver_set_entry *)self->entries->hh.next : NULL;
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

PHP7TO8_ARG_INFO_VOID_RETURN(arginfo_void_return)
PHP7TO8_ARG_INFO_BOOL_RETURN(arginfo_bool_return)
PHP7TO8_ARG_INFO_MIXED_RETURN(arginfo_mixed_return)
PHP7TO8_ARG_INFO_LONG_RETURN(arginfo_long_return)

static zend_function_entry php_driver_set_methods[] = {
  PHP_ME(Set, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Set, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, add, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Set, has, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Set, remove, arginfo_one, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(Set, count, arginfo_long_return, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(Set, current, arginfo_mixed_return, ZEND_ACC_PUBLIC)
  PHP_ME(Set, key, arginfo_mixed_return, ZEND_ACC_PUBLIC)
  PHP_ME(Set, next, arginfo_void_return, ZEND_ACC_PUBLIC)
  PHP_ME(Set, rewind, arginfo_void_return, ZEND_ACC_PUBLIC)
  PHP_ME(Set, valid, arginfo_bool_return, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_set_handlers;

static HashTable *
php_driver_set_gc(php7to8_object *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_set_properties(php7to8_object *object TSRMLS_DC)
{
  php5to7_zval values;

#if PHP_MAJOR_VERSION >= 8
  php_driver_set *self = PHP5TO7_ZEND_OBJECT_GET(set, object);
#else
  php_driver_set *self = PHP_DRIVER_GET_SET(object);
#endif
  HashTable     *props = zend_std_get_properties(object TSRMLS_CC);


  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "type", sizeof("type"),
                           PHP5TO7_ZVAL_MAYBE_P(self->type), sizeof(zval));
  Z_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->type));

  PHP5TO7_ZVAL_MAYBE_MAKE(values);
  array_init(PHP5TO7_ZVAL_MAYBE_P(values));
  php_driver_set_populate(self , PHP5TO7_ZVAL_MAYBE_P(values) TSRMLS_CC);
  PHP5TO7_ZEND_HASH_SORT(Z_ARRVAL_P(PHP5TO7_ZVAL_MAYBE_P(values)), php_driver_data_compare, 1);
  PHP5TO7_ZEND_HASH_UPDATE(props, "values", sizeof("values"), PHP5TO7_ZVAL_MAYBE_P(values), sizeof(zval));

  return props;
}

static int
php_driver_set_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  php_driver_set_entry *curr, *temp;
  php_driver_set *set1;
  php_driver_set *set2;
  php_driver_type *type1;
  php_driver_type *type2;
  int result;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  set1 = PHP_DRIVER_GET_SET(obj1);
  set2 = PHP_DRIVER_GET_SET(obj2);

  type1 = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set1->type));
  type2 = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set2->type));

  result = php_driver_type_compare(type1, type2 TSRMLS_CC);
  if (result != 0) return result;

  if (HASH_COUNT(set1->entries) != HASH_COUNT(set2->entries)) {
   return HASH_COUNT(set1->entries) < HASH_COUNT(set2->entries) ? -1 : 1;
  }

  HASH_ITER(hh, set1->entries, curr, temp) {
    php_driver_set_entry *entry;
    HASH_FIND_ZVAL(set2->entries, PHP5TO7_ZVAL_MAYBE_P(curr->value), entry);
    if (entry == NULL) {
      return 1;
    }
  }

  return 0;
}

static unsigned
php_driver_set_hash_value(zval *obj TSRMLS_DC)
{
  unsigned hashv = 0;
  php_driver_set_entry *curr,  *temp;
  php_driver_set *self = PHP_DRIVER_GET_SET(obj);

  if (!self->dirty) return self->hashv;

  HASH_ITER(hh, self->entries, curr, temp) {
    hashv = php_driver_combine_hash(hashv, php_driver_value_hash(PHP5TO7_ZVAL_MAYBE_P(curr->value) TSRMLS_CC));
  }

  self->hashv = hashv;
  self->dirty = 0;

  return hashv;
}

static void
php_driver_set_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_set *self = PHP5TO7_ZEND_OBJECT_GET(set, object);
  php_driver_set_entry *curr, *temp;

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
php_driver_set_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_set *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(set, ce);

  self->entries = self->iter_curr = self->iter_temp = NULL;
  self->iter_index = 0;
  self->dirty = 1;
  PHP5TO7_ZVAL_UNDEF(self->type);

  PHP5TO7_ZEND_OBJECT_INIT(set, self, ce);
}

void php_driver_define_Set(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Set", php_driver_set_methods);
  php_driver_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_set_ce TSRMLS_CC, 1, php_driver_value_ce);
  memcpy(&php_driver_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_set_handlers.std.get_properties  = php_driver_set_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_set_handlers.std.get_gc          = php_driver_set_gc;
#endif
  PHP7TO8_COMPARE(php_driver_set_handlers.std, php_driver_set_compare);
  php_driver_set_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_set_ce->create_object = php_driver_set_new;
  zend_class_implements(php_driver_set_ce TSRMLS_CC, 2, PHP7TO8_COUNTABLE, zend_ce_iterator);

  php_driver_set_handlers.hash_value = php_driver_set_hash_value;
  php_driver_set_handlers.std.clone_obj = NULL;
}
