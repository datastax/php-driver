#include "php_cassandra.h"
#include "util/collections.h"
#include "util/hash.h"
#include "util/types.h"
#include "src/Cassandra/Type/UserType.h"
#include "src/Cassandra/UserTypeValue.h"

zend_class_entry *cassandra_user_type_value_ce = NULL;

int
php_cassandra_user_type_value_set(cassandra_user_type_value *user_type_value,
                                  const char *name, size_t name_length,
                                  zval *object TSRMLS_DC)
{
  if (PHP5TO7_ZEND_HASH_UPDATE(&user_type_value->values,
                               name, name_length + 1,
                               object, sizeof(zval *))) {
    Z_TRY_ADDREF_P(object);
    user_type_value->dirty = 1;
    return 1;
  }
  return 0;
}

/* {{{ Cassandra\UserTypeValue::__construct(types) */
PHP_METHOD(UserTypeValue, __construct)
{
  cassandra_user_type_value *self;
  cassandra_type *type;
  HashTable *types;
  char *name;
  php5to7_zval *current;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "h", &types) == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());
  self->type = php_cassandra_type_user_type(TSRMLS_C);
  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(types, name, current) {
    zval *sub_type = PHP5TO7_ZVAL_MAYBE_DEREF(current);
    php5to7_zval scalar_type;

    if (Z_TYPE_P(sub_type) == IS_STRING) {
      CassValueType value_type;
      if (!php_cassandra_value_type(Z_STRVAL_P(sub_type), &value_type TSRMLS_CC)) {
        return;
      }
      scalar_type = php_cassandra_type_scalar(value_type TSRMLS_CC);
      if (!php_cassandra_type_user_type_add(type,
                                            name, strlen(name),
                                            PHP5TO7_ZVAL_MAYBE_P(scalar_type) TSRMLS_CC)) {
        return;
      }
    } else if (Z_TYPE_P(sub_type) == IS_OBJECT &&
               instanceof_function(Z_OBJCE_P(sub_type), cassandra_type_ce TSRMLS_CC)) {
      if (!php_cassandra_type_validate(sub_type, "sub_type" TSRMLS_CC)) {
        return;
      }
      if (php_cassandra_type_user_type_add(type,
                                           name, strlen(name),
                                           sub_type TSRMLS_CC)) {
        Z_ADDREF_P(sub_type);
      } else {
        return;
      }
    } else {
      INVALID_ARGUMENT(sub_type, "a string or an instance of Cassandra\\Type");
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(types);
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::type() */
PHP_METHOD(UserTypeValue, type)
{
  cassandra_user_type_value *self = PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(self->type), 1, 0);
}

/* {{{ Cassandra\UserTypeValue::values() */
PHP_METHOD(UserTypeValue, values)
{
  cassandra_user_type_value *self = NULL;
  array_init(return_value);
  self = PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());

  array_init(return_value);
  PHP5TO7_ZEND_HASH_ZVAL_COPY(Z_ARRVAL_P(return_value), &self->values);
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::set(name, mixed) */
PHP_METHOD(UserTypeValue, set)
{
  cassandra_user_type_value *self = NULL;
  cassandra_type *type;
  php5to7_zval *sub_type;
  char *name;
  size_t name_length;
  zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
                            &name, &name_length,
                            &value) == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());
  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(self->type));

  if (!PHP5TO7_ZEND_HASH_FIND(&type->types,
                              name, name_length + 1,
                              sub_type)) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid name '%s'", name);
    return;
  }

  if (!php_cassandra_validate_object(value,
                                     PHP5TO7_ZVAL_MAYBE_DEREF(sub_type) TSRMLS_CC)) {
    return;
  }

  php_cassandra_user_type_value_set(self, name, name_length, value TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::get(name) */
PHP_METHOD(UserTypeValue, get)
{
  cassandra_user_type_value *self = NULL;
  char *name;
  size_t name_length;
  php5to7_zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                            &name, &name_length) == FAILURE)
    return;

  self = PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());

  if (!PHP5TO7_ZEND_HASH_FIND(&self->values,
                              name, name_length + 1,
                              value)) {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                            "Invalid name '%s'", name);
    return;
  }

  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(value), 1, 0);
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::count() */
PHP_METHOD(UserTypeValue, count)
{
  cassandra_user_type_value *self =
      PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());
  RETURN_LONG(zend_hash_num_elements(&self->values));
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::current() */
PHP_METHOD(UserTypeValue, current)
{
  php5to7_zval *current;
  cassandra_user_type_value *self =
      PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());

  if (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(&self->values, current)) {
    RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_DEREF(current), 1, 0);
  }
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::key() */
PHP_METHOD(UserTypeValue, key)
{
  php5to7_string key;
  cassandra_user_type_value *self =
      PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());
  if (PHP5TO7_ZEND_HASH_GET_CURRENT_KEY(&self->values, &key, NULL) == HASH_KEY_IS_STRING) {
#if PHP_MAJOR_VERSION >= 7
    RETURN_STR(key);
#else
    RETURN_STRING(key, 1);
#endif
  }
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::next() */
PHP_METHOD(UserTypeValue, next)
{
  cassandra_user_type_value *self =
      PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());
  zend_hash_move_forward(&self->values);
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::valid() */
PHP_METHOD(UserTypeValue, valid)
{
  cassandra_user_type_value *self =
      PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());
  RETURN_BOOL(zend_hash_has_more_elements(&self->values) == SUCCESS);
}
/* }}} */

/* {{{ Cassandra\UserTypeValue::rewind() */
PHP_METHOD(UserTypeValue, rewind)
{
  cassandra_user_type_value *self =
      PHP_CASSANDRA_GET_USER_TYPE_VALUE(getThis());
  zend_hash_internal_pointer_reset(&self->values);
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

static zend_function_entry cassandra_user_type_value_methods[] = {
  PHP_ME(UserTypeValue, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, set, arginfo_value, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, get, arginfo_index, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(UserTypeValue, count, arginfo_none, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(UserTypeValue, current, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, key, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, next, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, valid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(UserTypeValue, rewind, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_cassandra_value_handlers cassandra_user_type_value_handlers;

static HashTable *
php_cassandra_user_type_value_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_cassandra_user_type_value_properties(zval *object TSRMLS_DC)
{
  cassandra_user_type_value *self = PHP_CASSANDRA_GET_USER_TYPE_VALUE(object);
  HashTable                 *props = zend_std_get_properties(object TSRMLS_CC);
  php5to7_zval              values;

  PHP5TO7_ZVAL_MAYBE_MAKE(values);
  array_init(PHP5TO7_ZVAL_MAYBE_P(values));
  PHP5TO7_ZEND_HASH_ZVAL_COPY(PHP5TO7_Z_ARRVAL_MAYBE_P(values), &self->values);

  PHP5TO7_ZEND_HASH_UPDATE(props, "values", sizeof("values"), PHP5TO7_ZVAL_MAYBE_P(values), sizeof(zval));

  return props;
}

static int
php_cassandra_user_type_value_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  HashPosition pos1;
  HashPosition pos2;
  php5to7_zval *current1;
  php5to7_zval *current2;
  cassandra_user_type_value *user_type_value1;
  cassandra_user_type_value *user_type_value2;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  user_type_value1 = PHP_CASSANDRA_GET_USER_TYPE_VALUE(obj1);
  user_type_value2 = PHP_CASSANDRA_GET_USER_TYPE_VALUE(obj2);

  if (zend_hash_num_elements(&user_type_value1->values) != zend_hash_num_elements(&user_type_value2->values)) {
    return zend_hash_num_elements(&user_type_value1->values) < zend_hash_num_elements(&user_type_value2->values) ? -1 : 1;
  }

  zend_hash_internal_pointer_reset_ex(&user_type_value1->values, &pos1);
  zend_hash_internal_pointer_reset_ex(&user_type_value2->values, &pos2);

#if PHP_MAJOR_VERSION >= 7
  while ((current1 = zend_hash_get_current_data_ex(&user_type_value1->values, &pos1)) != NULL &&
         (current2 = zend_hash_get_current_data_ex(&user_type_value1->values, &pos2)) != NULL) {
#else
  while (zend_hash_get_current_data(&user_type_value1->values, (void**) &current1) == SUCCESS &&
         zend_hash_get_current_data(&user_type_value2->values, (void**) &current2) == SUCCESS) {
#endif
    int r = php_cassandra_value_compare(PHP5TO7_ZVAL_MAYBE_DEREF(current1),
                                        PHP5TO7_ZVAL_MAYBE_DEREF(current2) TSRMLS_CC);
    if (r != 0) return r;
    zend_hash_move_forward_ex(&user_type_value1->values, &pos1);
    zend_hash_move_forward_ex(&user_type_value2->values, &pos2);
  }

  return 0;
}

static unsigned
php_cassandra_user_type_value_hash_value(zval *obj TSRMLS_DC)
{
  php5to7_zval *current;
  unsigned hashv = 0;
  cassandra_user_type_value *self = PHP_CASSANDRA_GET_USER_TYPE_VALUE(obj);

  if (!self->dirty) return self->hashv;

  PHP5TO7_ZEND_HASH_FOREACH_VAL(&self->values, current) {
    hashv = php_cassandra_combine_hash(hashv,
                                       php_cassandra_value_hash(PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC));
  } PHP5TO7_ZEND_HASH_FOREACH_END(&self->values);

  self->hashv = hashv;
  self->dirty = 0;

  return hashv;
}

static void
php_cassandra_user_type_value_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_user_type_value *self =
      PHP5TO7_ZEND_OBJECT_GET(user_type_value, object);

  zend_hash_destroy(&self->values);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->type);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_user_type_value_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_user_type_value *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(user_type_value, ce);

  zend_hash_init(&self->values, 0, NULL, ZVAL_PTR_DTOR, 0);
  self->dirty = 1;
  PHP5TO7_ZVAL_UNDEF(self->type);

  PHP5TO7_ZEND_OBJECT_INIT(user_type_value, self, ce);
}

void cassandra_define_UserTypeValue(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\UserTypeValue", cassandra_user_type_value_methods);
  cassandra_user_type_value_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_user_type_value_ce TSRMLS_CC, 1, cassandra_value_ce);
  memcpy(&cassandra_user_type_value_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_user_type_value_handlers.std.get_properties  = php_cassandra_user_type_value_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_user_type_value_handlers.std.get_gc          = php_cassandra_user_type_value_gc;
#endif
  cassandra_user_type_value_handlers.std.compare_objects = php_cassandra_user_type_value_compare;
  cassandra_user_type_value_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_user_type_value_ce->create_object = php_cassandra_user_type_value_new;
  zend_class_implements(cassandra_user_type_value_ce TSRMLS_CC, 2, spl_ce_Countable, zend_ce_iterator);

  cassandra_user_type_value_handlers.hash_value = php_cassandra_user_type_value_hash_value;
}
