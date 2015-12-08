#include "php_cassandra.h"
#include "util/math.h"
#include "util/types.h"

#if !defined(HAVE_STDINT_H) && !defined(_MSC_STDINT_H_)
#  define INT64_MAX 9223372036854775807LL
#  define INT64_MIN (-INT_MAX-1)
#endif

zend_class_entry* cassandra_bigint_ce = NULL;

static int
to_double(zval* result, cassandra_bigint* bigint TSRMLS_DC)
{
  ZVAL_DOUBLE(result, (double) bigint->value);
  return SUCCESS;
}

static int
to_long(zval* result, cassandra_bigint* bigint TSRMLS_DC)
{
  if (bigint->value < (cass_int64_t) LONG_MIN) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    return FAILURE;
  }

  if (bigint->value > (cass_int64_t) LONG_MAX) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    return FAILURE;
  }

  ZVAL_LONG(result, (long) bigint->value);
  return SUCCESS;
}

static int
to_string(zval* result, cassandra_bigint* bigint TSRMLS_DC)
{
  char* string;
#ifdef WIN32
  spprintf(&string, 0, "%I64d", (long long int) bigint->value);
#else
  spprintf(&string, 0, "%lld", (long long int) bigint->value);
#endif
  ZVAL_STRING(result, string, 0);
  return SUCCESS;
}

void
php_cassandra_bigint_init(INTERNAL_FUNCTION_PARAMETERS)
{
  cassandra_bigint* self;
  zval* value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_bigint_ce TSRMLS_CC)) {
    self = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
  } else {
    object_init_ex(return_value, cassandra_bigint_ce);
    self = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  }

  if (Z_TYPE_P(value) == IS_LONG) {
    self->value = (cass_int64_t) Z_LVAL_P(value);
  } else if (Z_TYPE_P(value) == IS_DOUBLE) {
    self->value = (cass_int64_t) Z_DVAL_P(value);
  } else if (Z_TYPE_P(value) == IS_STRING) {
    if (!php_cassandra_parse_bigint(Z_STRVAL_P(value), Z_STRLEN_P(value),
                                    &self->value TSRMLS_CC)) {
      return;
    }
  } else if (Z_TYPE_P(value) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(value), cassandra_bigint_ce TSRMLS_CC)) {
    cassandra_bigint* bigint = (cassandra_bigint*)
                                 zend_object_store_get_object(value TSRMLS_CC);
    self->value = bigint->value;
  } else {
    INVALID_ARGUMENT(value, "a long, a double, a numeric string or a " \
                            "Cassandra\\Bigint");
  }
}

/* {{{ Cassandra\Bigint::__construct(string) */
PHP_METHOD(Bigint, __construct)
{
  php_cassandra_bigint_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Bigint::__toString() */
PHP_METHOD(Bigint, __toString)
{
  cassandra_bigint* self = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  to_string(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Bigint::type() */
PHP_METHOD(Bigint, type)
{
  cassandra_bigint* self = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->type, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Bigint::value() */
PHP_METHOD(Bigint, value)
{
  cassandra_bigint* self = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  to_string(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Bigint::add() */
PHP_METHOD(Bigint, add)
{
  zval* num;
  cassandra_bigint* self;
  cassandra_bigint* bigint;
  cassandra_bigint* result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce TSRMLS_CC)) {
    self = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    bigint = (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    result = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value + bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Bigint::sub() */
PHP_METHOD(Bigint, sub)
{
  zval* num;
  cassandra_bigint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce TSRMLS_CC)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    result = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value - bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Bigint::mul() */
PHP_METHOD(Bigint, mul)
{
  zval* num;
  cassandra_bigint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce TSRMLS_CC)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    result = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value * bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Bigint::div() */
PHP_METHOD(Bigint, div)
{
  zval* num;
  cassandra_bigint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce TSRMLS_CC)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    result = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (bigint->value == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot divide by zero");
      return;
    }

    result->value = self->value / bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Bigint::mod() */
PHP_METHOD(Bigint, mod)
{
  zval* num;
  cassandra_bigint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce TSRMLS_CC)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    result = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (bigint->value == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot modulo by zero");
      return;
    }

    result->value = self->value % bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Bigint::abs() */
PHP_METHOD(Bigint, abs)
{
  cassandra_bigint* result = NULL;

  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->value == INT64_MIN) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value doesn't exist");
    return;
  }

  object_init_ex(return_value, cassandra_bigint_ce);
  result = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = self->value < 0 ? -self->value : self->value;
}
/* }}} */

/* {{{ Cassandra\Bigint::neg() */
PHP_METHOD(Bigint, neg)
{
  cassandra_bigint* result = NULL;

  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_bigint_ce);
  result = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = -self->value;
}
/* }}} */

/* {{{ Cassandra\Bigint::sqrt() */
PHP_METHOD(Bigint, sqrt)
{
  cassandra_bigint* result = NULL;

  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->value < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC,
                            "Cannot take a square root of a negative number");
  }

  object_init_ex(return_value, cassandra_bigint_ce);
  result = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = (cass_int64_t) sqrt((long double) self->value);
}
/* }}} */

/* {{{ Cassandra\Bigint::toInt() */
PHP_METHOD(Bigint, toInt)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  to_long(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Bigint::toDouble() */
PHP_METHOD(Bigint, toDouble)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  to_double(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Bigint::min() */
PHP_METHOD(Bigint, min)
{
  cassandra_bigint* bigint = NULL;
  object_init_ex(return_value, cassandra_bigint_ce);
  bigint = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  bigint->value = INT64_MIN;
}
/* }}} */

/* {{{ Cassandra\Bigint::max() */
PHP_METHOD(Bigint, max)
{
  cassandra_bigint* bigint = NULL;
  object_init_ex(return_value, cassandra_bigint_ce);
  bigint = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  bigint->value = INT64_MAX;
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_num, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_bigint_methods[] = {
  PHP_ME(Bigint, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, mod, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, toInt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, min, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_ME(Bigint, max, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_bigint_handlers;

static HashTable*
php_cassandra_bigint_gc(zval *object, zval ***table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_bigint_properties(zval *object TSRMLS_DC)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*        props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  int string_len;

#ifdef WIN32
  string_len = spprintf(&string, 0, "%I64d", (long long int) self->value);
#else
  string_len = spprintf(&string, 0, "%lld", (long long int) self->value);
#endif

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_bigint_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_bigint* bigint1 = NULL;
  cassandra_bigint* bigint2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  bigint1 = (cassandra_bigint*) zend_object_store_get_object(obj1 TSRMLS_CC);
  bigint2 = (cassandra_bigint*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (bigint1->value == bigint2->value)
    return 0;
  else if (bigint1->value < bigint2->value)
    return -1;
  else
    return 1;
}

static int
php_cassandra_bigint_cast(zval* object, zval* retval, int type TSRMLS_DC)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(object TSRMLS_CC);

  switch (type) {
  case IS_LONG:
      return to_long(retval, self TSRMLS_CC);
  case IS_DOUBLE:
      return to_double(retval, self TSRMLS_CC);
  case IS_STRING:
      return to_string(retval, self TSRMLS_CC);
  default:
     return FAILURE;
  }

  return SUCCESS;
}

static void
php_cassandra_bigint_free(void *object TSRMLS_DC)
{
  cassandra_bigint* self = (cassandra_bigint*) object;

  zval_ptr_dtor(&self->type);
  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_bigint_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_bigint *self;

  self = (cassandra_bigint*) emalloc(sizeof(cassandra_bigint));
  memset(self, 0, sizeof(cassandra_bigint));

  self->type = php_cassandra_type_scalar(CASS_VALUE_TYPE_BIGINT TSRMLS_CC);
  Z_ADDREF_P(self->type);

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  retval.handle   = zend_objects_store_put(self, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_bigint_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_bigint_handlers;

  return retval;
}

void cassandra_define_Bigint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Bigint", cassandra_bigint_methods);
  cassandra_bigint_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_bigint_ce TSRMLS_CC, 1, cassandra_numeric_ce);
  cassandra_bigint_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_bigint_ce->create_object = php_cassandra_bigint_new;

  memcpy(&cassandra_bigint_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_bigint_handlers.get_properties  = php_cassandra_bigint_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_bigint_handlers.get_gc          = php_cassandra_bigint_gc;
#endif
  cassandra_bigint_handlers.compare_objects = php_cassandra_bigint_compare;
  cassandra_bigint_handlers.cast_object     = php_cassandra_bigint_cast;
}
