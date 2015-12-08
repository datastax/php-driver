#include "php_cassandra.h"
#include "util/math.h"
#include "util/types.h"
#include <float.h>

zend_class_entry* cassandra_float_ce = NULL;

static int
to_string(zval* result, cassandra_float* flt TSRMLS_DC)
{
  char* string;
  spprintf(&string, 0, "%.*F", (int) EG(precision), flt->value);
  ZVAL_STRING(result, string, 0);
  return SUCCESS;
}

void
php_cassandra_float_init(INTERNAL_FUNCTION_PARAMETERS)
{
  cassandra_float* self;
  zval* value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_float_ce TSRMLS_CC)) {
    self = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  } else {
    object_init_ex(return_value, cassandra_float_ce);
    self = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  }

  if (Z_TYPE_P(value) == IS_LONG) {
    self->value = (cass_float_t) Z_LVAL_P(value);
  } else if (Z_TYPE_P(value) == IS_DOUBLE) {
    self->value = (cass_float_t) Z_DVAL_P(value);
  } else if (Z_TYPE_P(value) == IS_STRING) {
    if (!php_cassandra_parse_float(Z_STRVAL_P(value), Z_STRLEN_P(value),
                                   &self->value TSRMLS_CC)) {
      return;
    }
  } else if (Z_TYPE_P(value) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(value), cassandra_float_ce TSRMLS_CC)) {
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
    self->value = flt->value;
  } else {
    INVALID_ARGUMENT(value, "a long, double, numeric string or a " \
                            "Cassandra\\Float instance");
  }
}

/* {{{ Cassandra\Float::__construct(string) */
PHP_METHOD(Float, __construct)
{
  php_cassandra_float_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Float::__toString() */
PHP_METHOD(Float, __toString)
{
  cassandra_float* self = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);

  to_string(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Float::type() */
PHP_METHOD(Float, type)
{
  cassandra_float* self = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->type, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Float::value() */
PHP_METHOD(Float, value)
{
  cassandra_float* self = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_DOUBLE((double) self->value);
}
/* }}} */

/* {{{ Cassandra\Float::isInfinite() */
PHP_METHOD(Float, isInfinite)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_isinf(self->value));
}
/* }}} */

/* {{{ Cassandra\Float::isFinite() */
PHP_METHOD(Float, isFinite)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_finite(self->value));
}
/* }}} */

/* {{{ Cassandra\Float::isNaN() */
PHP_METHOD(Float, isNaN)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_isnan(self->value));
}
/* }}} */

/* {{{ Cassandra\Float::add() */
PHP_METHOD(Float, add)
{
  zval* num;
  cassandra_float* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
    cassandra_float* self =
        (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_float_ce);
    result = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value + flt->value;
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Float::sub() */
PHP_METHOD(Float, sub)
{
  zval* num;
  cassandra_float* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
    cassandra_float* self =
        (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_float_ce);
    result = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value - flt->value;
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Float::mul() */
PHP_METHOD(Float, mul)
{
  zval* num;
  cassandra_float* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
    cassandra_float* self =
        (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_float_ce);
    result = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value * flt->value;
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Float::div() */
PHP_METHOD(Float, div)
{
  zval* num;
  cassandra_float* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
    cassandra_float* self =
        (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_float_ce);
    result = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (flt->value == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot divide by zero");
      return;
    }

    result->value = self->value / flt->value;
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Float::mod() */
PHP_METHOD(Float, mod)
{
  /* TODO: We could use fmod() here, but maybe we should add a remainder function
   * for floating point types.
   */
  zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC, "Not implemented");
}

/* {{{ Cassandra\Float::abs() */
PHP_METHOD(Float, abs)
{
  cassandra_float* result = NULL;

  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  object_init_ex(return_value, cassandra_float_ce);
  result = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = fabsf(self->value);
}
/* }}} */

/* {{{ Cassandra\Float::neg() */
PHP_METHOD(Float, neg)
{
  cassandra_float* result = NULL;

  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  object_init_ex(return_value, cassandra_float_ce);
  result = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = -self->value;
}
/* }}} */

/* {{{ Cassandra\Float::sqrt() */
PHP_METHOD(Float, sqrt)
{
  cassandra_float* result = NULL;

  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->value < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC,
                            "Cannot take a square root of a negative number");
  }

  object_init_ex(return_value, cassandra_float_ce);
  result = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = sqrtf(self->value);
}
/* }}} */

/* {{{ Cassandra\Float::toInt() */
PHP_METHOD(Float, toInt)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_LONG((long) self->value);
}
/* }}} */

/* {{{ Cassandra\Float::toDouble() */
PHP_METHOD(Float, toDouble)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_DOUBLE((double) self->value);
}
/* }}} */

/* {{{ Cassandra\Float::min() */
PHP_METHOD(Float, min)
{
  cassandra_float* flt = NULL;
  object_init_ex(return_value, cassandra_float_ce);
  flt = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  flt->value = FLT_MIN;
}
/* }}} */

/* {{{ Cassandra\Float::max() */
PHP_METHOD(Float, max)
{
  cassandra_float* flt = NULL;
  object_init_ex(return_value, cassandra_float_ce);
  flt = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  flt->value = FLT_MAX;
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

static zend_function_entry cassandra_float_methods[] = {
  PHP_ME(Float, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Float, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, isInfinite, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, isFinite, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, isNaN, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, mod, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, toInt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, min, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_ME(Float, max, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_float_handlers;

static HashTable*
php_cassandra_float_gc(zval *object, zval ***table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_float_properties(zval *object TSRMLS_DC)
{
  cassandra_float* self = (cassandra_float*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*       props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  int string_len;

  string_len = spprintf(&string, 0, "%.*F", (int) EG(precision), self->value);

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_float_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_float* flt1 = NULL;
  cassandra_float* flt2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  flt1 = (cassandra_float*) zend_object_store_get_object(obj1 TSRMLS_CC);
  flt2 = (cassandra_float*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (flt1->value == flt2->value)
    return 0;
  else if (flt1->value < flt2->value)
    return -1;
  else
    return 1;
}

static int
php_cassandra_float_cast(zval* object, zval* retval, int type TSRMLS_DC)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(object TSRMLS_CC);

  switch (type) {
  case IS_LONG:
      ZVAL_LONG(retval, (long) self->value);
      return SUCCESS;
  case IS_DOUBLE:
      ZVAL_DOUBLE(retval, (double) self->value);
      return SUCCESS;
  case IS_STRING:
      return to_string(retval, self TSRMLS_CC);
  default:
     return FAILURE;
  }

  return SUCCESS;
}

static void
php_cassandra_float_free(void *object TSRMLS_DC)
{
  cassandra_float* self = (cassandra_float*) object;

  zval_ptr_dtor(&self->type);
  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_float_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_float *self;

  self = (cassandra_float*) emalloc(sizeof(cassandra_float));
  memset(self, 0, sizeof(cassandra_float));

  self->type = php_cassandra_type_scalar(CASS_VALUE_TYPE_FLOAT TSRMLS_CC);
  Z_ADDREF_P(self->type);

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  retval.handle   = zend_objects_store_put(self, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_float_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_float_handlers;

  return retval;
}

void cassandra_define_Float(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Float", cassandra_float_methods);
  cassandra_float_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_float_ce TSRMLS_CC, 1, cassandra_numeric_ce);
  cassandra_float_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_float_ce->create_object = php_cassandra_float_new;

  memcpy(&cassandra_float_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_float_handlers.get_properties  = php_cassandra_float_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_float_handlers.get_gc          = php_cassandra_float_gc;
#endif
  cassandra_float_handlers.compare_objects = php_cassandra_float_compare;
  cassandra_float_handlers.cast_object     = php_cassandra_float_cast;
}
