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

#include "src/Type/UserType.h"
#include "src/UserTypeValue.h"

zend_class_entry *php_driver_user_type_value_ce = NULL;

void
php_driver_user_type_value_set(php_driver_user_type_value *user_type_value,
                                  const char *name, size_t name_length,
                                  zval *object TSRMLS_DC)
{
  PHP5TO7_ZEND_HASH_UPDATE(&user_type_value->values,
                           name, name_length + 1,
                           object, sizeof(zval *));
  Z_TRY_ADDREF_P(object);
  user_type_value->dirty = 1;
}

static void
php_driver_user_type_value_populate(php_driver_user_type_value *user_type_value, zval *array TSRMLS_DC)
{
  char *name;
  php_driver_type *type;
  php5to7_zval *current;
  php5to7_zval null;

  PHP5TO7_ZVAL_MAYBE_MAKE(null);
  ZVAL_NULL(PHP5TO7_ZVAL_MAYBE_P(null));

  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(user_type_value->type));

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(&type->data.udt.types, name, current) {
    php5to7_zval *value = NULL;
    size_t name_len = strlen(name);
    (void) current;
    if (PHP5TO7_ZEND_HASH_FIND(&user_type_value->values, name, name_len + 1, value)) {
      PHP5TO7_ADD_ASSOC_ZVAL_EX(array, name, name_len + 1, PHP5TO7_ZVAL_MAYBE_DEREF(value));
      Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_DEREF(value));
    } else {
      PHP5TO7_ADD_ASSOC_ZVAL_EX(array, name, name_len + 1, PHP5TO7_ZVAL_MAYBE_P(null));
      Z_TRY_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(null));
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(&type->data.udt.types);

#if PHP_MAJOR_VERSION < 7
  zval_ptr_dtor(&null);
#endif
}

/* {{{ UserTypeValue::__construct(types) */
PHP_METHOD(UserTypeValue, __construct)
{
  php_driver_user_type_value *self;
  php_driver_type *type;
  HashTable *types;
  char *name;
  int index = 0;
  php5to7_zval *current;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "h", &types) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  self->type = php_driver_type_user_type(TSRMLS_C);
  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(types, name, current) {
    zval *sub_type = PHP5TO7_ZVAL_MAYBE_DEREF(current);
    php5to7_zval scalar_type;

    if (!name) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "Argument %d is not a string", index + 1);
      return;
    }
    index++;

    if (Z_TYPE_P(sub_type) == IS_STRING) {
      CassValueType value_type;
      if (!php_driver_value_type(Z_STRVAL_P(sub_type), &value_type TSRMLS_CC)) {
        return;
      }
      scalar_type = php_driver_type_scalar(value_type TSRMLS_CC);
      if (!php_driver_type_user_type_add(type,
                                            name, strlen(name),
                                            PHP5TO7_ZVAL_MAYBE_P(scalar_type) TSRMLS_CC)) {
        return;
      }
    } else if (Z_TYPE_P(sub_type) == IS_OBJECT &&
               instanceof_function(Z_OBJCE_P(sub_type), php_driver_type_ce TSRMLS_CC)) {
      if (!php_driver_type_validate(sub_type, "sub_type" TSRMLS_CC)) {
        return;
      }
      if (php_driver_type_user_type_add(type,
                                           name, strlen(name),
                                           sub_type TSRMLS_CC)) {
        Z_ADDREF_P(sub_type);
      } else {
        return;
      }
    } else {
      INVALID_ARGUMENT(sub_type, "a string or an instance of " PHP_DRIVER_NAMESPACE "\\Type");
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(types);
}
/* }}} */

/* {{{ UserTypeValue::type() */
PHP_METHOD(UserTypeValue, type)
{
  php_driver_user_type_value *self = PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->type), 1, 0);
}

/* {{{ UserTypeValue::values() */
PHP_METHOD(UserTypeValue, values)
{
  php_driver_user_type_value *self = NULL;
  self = PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());

  array_init(return_value);
  php_driver_user_type_value_populate(self, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ UserTypeValue::set(name, mixed) */
PHP_METHOD(UserTypeValue, set)
{
  php_driver_user_type_value *self = NULL;
  php_driver_type *type;
  php5to7_zval *sub_type;
  char *name;
  size_t name_length;
  zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                            &name, &name_length,
                            &value) == FAILURE)
    return;

  self = PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));

  if (!PHP5TO7_ZEND_HASH_FIND(&type->data.udt.types,
                              name, name_length + 1,
                              sub_type)) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid name '%s'", name);
    return;
  }

  if (!php_driver_validate_object(value,
                                     PHP5TO7_ZVAL_MAYBE_DEREF(sub_type) TSRMLS_CC)) {
    return;
  }

  php_driver_user_type_value_set(self, name, name_length, value TSRMLS_CC);
}
/* }}} */

/* {{{ UserTypeValue::get(name) */
PHP_METHOD(UserTypeValue, get)
{
  php_driver_user_type_value *self = NULL;
  php_driver_type *type;
  php5to7_zval *sub_type;
  char *name;
  size_t name_length;
  php5to7_zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_length) == FAILURE)
    return;

  self = PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  type = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));

  if (!PHP5TO7_ZEND_HASH_FIND(&type->data.udt.types,
                              name, name_length + 1,
                              sub_type)) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid name '%s'", name);
    return;
  }

  if (PHP5TO7_ZEND_HASH_FIND(&self->values,
                              name, name_length + 1,
                              value)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(value), 1, 0);
  }
}
/* }}} */

/* {{{ UserTypeValue::count() */
PHP_METHOD(UserTypeValue, count)
{
  php_driver_user_type_value *self =
      PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  php_driver_type *type =
      PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));
  RETURN_LONG(zend_hash_num_elements(&type->data.udt.types));
}
/* }}} */

/* {{{ UserTypeValue::current() */
PHP_METHOD(UserTypeValue, current)
{
  php5to7_string key;
  php_driver_user_type_value *self =
      PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  php_driver_type *type =
      PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));
  if (PHP5TO7_ZEND_HASH_GET_CURRENT_KEY_EX(&type->data.udt.types, &key, NULL, &self->pos) == HASH_KEY_IS_STRING) {
    php5to7_zval *value;
#if PHP_MAJOR_VERSION >= 7
    if (PHP5TO7_ZEND_HASH_FIND(&self->values, key->val, key->len + 1, value)) {
#else
    if (PHP5TO7_ZEND_HASH_FIND(&self->values, key, strlen(key) + 1, value)) {
#endif
      RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(value), 1, 0);
    }
  }
}
/* }}} */

/* {{{ UserTypeValue::key() */
PHP_METHOD(UserTypeValue, key)
{
  php5to7_string key;
  php_driver_user_type_value *self =
      PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  php_driver_type *type =
      PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));
  if (PHP5TO7_ZEND_HASH_GET_CURRENT_KEY_EX(&type->data.udt.types, &key, NULL, &self->pos) == HASH_KEY_IS_STRING) {
#if PHP_MAJOR_VERSION >= 7
    RETURN_STR(key);
#else
    RETURN_STRING(key, 1);
#endif
  }
}
/* }}} */

/* {{{ UserTypeValue::next() */
PHP_METHOD(UserTypeValue, next)
{
  php_driver_user_type_value *self =
      PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  php_driver_type *type =
      PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));
  zend_hash_move_forward_ex(&type->data.udt.types, &self->pos);
}
/* }}} */

/* {{{ UserTypeValue::valid() */
PHP_METHOD(UserTypeValue, valid)
{
  php_driver_user_type_value *self =
      PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  php_driver_type *type =
      PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));
  RETURN_BOOL(zend_hash_has_more_elements_ex(&type->data.udt.types, &self->pos) == SUCCESS);
}
/* }}} */

/* {{{ UserTypeValue::rewind() */
PHP_METHOD(UserTypeValue, rewind)
{
  php_driver_user_type_value *self =
      PHP_DRIVER_GET_USER_TYPE_VALUE(getThis());
  php_driver_type *type =
      PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));
  zend_hash_internal_pointer_reset_ex(&type->data.udt.types, &self->pos);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, types)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_string_mixed, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

PHP7TO8_ARG_INFO_VOID_RETURN(arginfo_void_return)
PHP7TO8_ARG_INFO_BOOL_RETURN(arginfo_bool_return)
PHP7TO8_ARG_INFO_MIXED_RETURN(arginfo_mixed_return)
PHP7TO8_ARG_INFO_LONG_RETURN(arginfo_long_return)

static zend_function_entry php_driver_user_type_value_methods[] = {
  PHP_ME(UserTypeValue, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, set, arginfo_string_mixed, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, get, arginfo_name, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(UserTypeValue, count, arginfo_long_return, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(UserTypeValue, current, arginfo_mixed_return, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, key, arginfo_mixed_return, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, next, arginfo_void_return, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, rewind, arginfo_void_return, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, valid, arginfo_bool_return, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_user_type_value_handlers;

static HashTable *
php_driver_user_type_value_gc(php7to8_object *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_user_type_value_properties(php7to8_object *object TSRMLS_DC)
{
  php5to7_zval values;

#if PHP_MAJOR_VERSION >= 8
  php_driver_user_type_value *self = PHP5TO7_ZEND_OBJECT_GET(user_type_value, object);
#else
  php_driver_user_type_value *self = PHP_DRIVER_GET_USER_TYPE_VALUE(object);
#endif
  HashTable                 *props = zend_std_get_properties(object TSRMLS_CC);

  PHP5TO7_ZEND_HASH_UPDATE(props,
                           "type", sizeof("type"),
                           PHP5TO7_ZVAL_MAYBE_P(self->type), sizeof(zval));
  Z_ADDREF_P(PHP5TO7_ZVAL_MAYBE_P(self->type));

  PHP5TO7_ZVAL_MAYBE_MAKE(values);
  array_init(PHP5TO7_ZVAL_MAYBE_P(values));
  php_driver_user_type_value_populate(self, PHP5TO7_ZVAL_MAYBE_P(values) TSRMLS_CC);
  PHP5TO7_ZEND_HASH_UPDATE(props, "values", sizeof("values"), PHP5TO7_ZVAL_MAYBE_P(values), sizeof(zval));

  return props;
}

static int
php_driver_user_type_value_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  HashPosition pos1;
  HashPosition pos2;
  php5to7_zval *current1;
  php5to7_zval *current2;
  php_driver_user_type_value *user_type_value1;
  php_driver_user_type_value *user_type_value2;
  php_driver_type *type1;
  php_driver_type *type2;
  int result;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  user_type_value1 = PHP_DRIVER_GET_USER_TYPE_VALUE(obj1);
  user_type_value2 = PHP_DRIVER_GET_USER_TYPE_VALUE(obj2);

  type1 = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(user_type_value1->type));
  type2 = PHP_DRIVER_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(user_type_value2->type));

  result = php_driver_type_compare(type1, type2 TSRMLS_CC);
  if (result != 0) return result;

  if (zend_hash_num_elements(&user_type_value1->values) != zend_hash_num_elements(&user_type_value2->values)) {
    return zend_hash_num_elements(&user_type_value1->values) < zend_hash_num_elements(&user_type_value2->values) ? -1 : 1;
  }

  zend_hash_internal_pointer_reset_ex(&user_type_value1->values, &pos1);
  zend_hash_internal_pointer_reset_ex(&user_type_value2->values, &pos2);

  while (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(&user_type_value1->values, current1, &pos1) &&
         PHP5TO7_ZEND_HASH_GET_CURRENT_DATA_EX(&user_type_value2->values, current2, &pos2)) {
    result = php_driver_value_compare(PHP5TO7_ZVAL_MAYBE_DEREF(current1),
                                         PHP5TO7_ZVAL_MAYBE_DEREF(current2) TSRMLS_CC);
    if (result != 0) return result;
    zend_hash_move_forward_ex(&user_type_value1->values, &pos1);
    zend_hash_move_forward_ex(&user_type_value2->values, &pos2);
  }

  return 0;
}

static unsigned
php_driver_user_type_value_hash_value(zval *obj TSRMLS_DC)
{
  php5to7_zval *current;
  unsigned hashv = 0;
  php_driver_user_type_value *self = PHP_DRIVER_GET_USER_TYPE_VALUE(obj);

  if (!self->dirty) return self->hashv;

  PHP5TO7_ZEND_HASH_FOREACH_VAL(&self->values, current) {
    hashv = php_driver_combine_hash(hashv,
                                       php_driver_value_hash(PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC));
  } PHP5TO7_ZEND_HASH_FOREACH_END(&self->values);

  self->hashv = hashv;
  self->dirty = 0;

  return hashv;
}

static void
php_driver_user_type_value_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_user_type_value *self =
      PHP5TO7_ZEND_OBJECT_GET(user_type_value, object);

  zend_hash_destroy(&self->values);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->type);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_user_type_value_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_user_type_value *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(user_type_value, ce);

  zend_hash_init(&self->values, 0, NULL, ZVAL_PTR_DTOR, 0);
#if PHP_MAJOR_VERSION >= 7
  self->pos = HT_INVALID_IDX;
#else
  self->pos = NULL;
#endif
  self->dirty = 1;
  PHP5TO7_ZVAL_UNDEF(self->type);

  PHP5TO7_ZEND_OBJECT_INIT(user_type_value, self, ce);
}

void php_driver_define_UserTypeValue(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\UserTypeValue", php_driver_user_type_value_methods);
  php_driver_user_type_value_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_user_type_value_ce TSRMLS_CC, 1, php_driver_value_ce);
  memcpy(&php_driver_user_type_value_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_user_type_value_handlers.std.get_properties  = php_driver_user_type_value_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_user_type_value_handlers.std.get_gc          = php_driver_user_type_value_gc;
#endif
  PHP7TO8_COMPARE(php_driver_user_type_value_handlers.std, php_driver_user_type_value_compare);
  php_driver_user_type_value_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_user_type_value_ce->create_object = php_driver_user_type_value_new;
  zend_class_implements(php_driver_user_type_value_ce TSRMLS_CC, 2, PHP7TO8_COUNTABLE, zend_ce_iterator);

  php_driver_user_type_value_handlers.hash_value = php_driver_user_type_value_hash_value;
  php_driver_user_type_value_handlers.std.clone_obj = NULL;
}
