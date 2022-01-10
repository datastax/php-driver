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
#include "util/math.h"
#include "util/types.h"
#include <float.h>

zend_class_entry *php_driver_float_ce = NULL;

static int
to_string(zval *result, php_driver_numeric *flt TSRMLS_DC)
{
  char *string;
  spprintf(&string, 0, "%.*F", (int) EG(precision), flt->data.floating.value);
  PHP5TO7_ZVAL_STRING(result, string);
  efree(string);
  return SUCCESS;
}

void
php_driver_float_init(INTERNAL_FUNCTION_PARAMETERS)
{
  php_driver_numeric *self;
  zval *value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), php_driver_float_ce TSRMLS_CC)) {
    self = PHP_DRIVER_GET_NUMERIC(getThis());
  } else {
    object_init_ex(return_value, php_driver_float_ce);
    self = PHP_DRIVER_GET_NUMERIC(return_value);
  }

  if (Z_TYPE_P(value) == IS_LONG) {
    self->data.floating.value = (cass_float_t) Z_LVAL_P(value);
  } else if (Z_TYPE_P(value) == IS_DOUBLE) {
    self->data.floating.value = (cass_float_t) Z_DVAL_P(value);
  } else if (Z_TYPE_P(value) == IS_STRING) {
    if (!php_driver_parse_float(Z_STRVAL_P(value), Z_STRLEN_P(value),
                                   &self->data.floating.value TSRMLS_CC)) {
      return;
    }
  } else if (Z_TYPE_P(value) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(value), php_driver_float_ce TSRMLS_CC)) {
    php_driver_numeric *flt = PHP_DRIVER_GET_NUMERIC(return_value);
    self->data.floating.value = flt->data.floating.value;
  } else {
    INVALID_ARGUMENT(value, "a long, double, numeric string or a " \
                            PHP_DRIVER_NAMESPACE "\\Float instance");
  }
}

/* {{{ Float::__construct(string) */
PHP_METHOD(Float, __construct)
{
  php_driver_float_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Float::__toString() */
PHP_METHOD(Float, __toString)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_string(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Float::type() */
PHP_METHOD(Float, type)
{
  php5to7_zval type = php_driver_type_scalar(CASS_VALUE_TYPE_FLOAT TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(type), 1, 1);
}
/* }}} */

/* {{{ Float::value() */
PHP_METHOD(Float, value)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
  RETURN_DOUBLE((double) self->data.floating.value);
}
/* }}} */

/* {{{ Float::isInfinite() */
PHP_METHOD(Float, isInfinite)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
  RETURN_BOOL(zend_isinf(self->data.floating.value));
}
/* }}} */

/* {{{ Float::isFinite() */
PHP_METHOD(Float, isFinite)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
  RETURN_BOOL(zend_finite(self->data.floating.value));
}
/* }}} */

/* {{{ Float::isNaN() */
PHP_METHOD(Float, isNaN)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
  RETURN_BOOL(zend_isnan(self->data.floating.value));
}
/* }}} */

/* {{{ Float::add() */
PHP_METHOD(Float, add)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_float_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *flt = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_float_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    result->data.floating.value = self->data.floating.value + flt->data.floating.value;
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Float");
  }
}
/* }}} */

/* {{{ Float::sub() */
PHP_METHOD(Float, sub)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_float_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *flt = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_float_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    result->data.floating.value = self->data.floating.value - flt->data.floating.value;
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Float");
  }
}
/* }}} */

/* {{{ Float::mul() */
PHP_METHOD(Float, mul)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_float_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *flt = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_float_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    result->data.floating.value = self->data.floating.value * flt->data.floating.value;
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Float");
  }
}
/* }}} */

/* {{{ Float::div() */
PHP_METHOD(Float, div)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_float_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *flt = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_float_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    if (flt->data.floating.value == 0) {
      zend_throw_exception_ex(php_driver_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot divide by zero");
      return;
    }

    result->data.floating.value = self->data.floating.value / flt->data.floating.value;
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Float");
  }
}
/* }}} */

/* {{{ Float::mod() */
PHP_METHOD(Float, mod)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_float_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *flt = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_float_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    if (flt->data.floating.value == 0) {
      zend_throw_exception_ex(php_driver_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot divide by zero");
      return;
    }

    result->data.floating.value = fmod(self->data.floating.value, flt->data.floating.value);
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Float");
  }
}

/* {{{ Float::abs() */
PHP_METHOD(Float, abs)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
  object_init_ex(return_value, php_driver_float_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);
  result->data.floating.value = fabsf(self->data.floating.value);
}
/* }}} */

/* {{{ Float::neg() */
PHP_METHOD(Float, neg)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
  object_init_ex(return_value, php_driver_float_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);
  result->data.floating.value = -self->data.floating.value;
}
/* }}} */

/* {{{ Float::sqrt() */
PHP_METHOD(Float, sqrt)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  if (self->data.floating.value < 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
                            "Cannot take a square root of a negative number");
  }

  object_init_ex(return_value, php_driver_float_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);
  result->data.floating.value = sqrtf(self->data.floating.value);
}
/* }}} */

/* {{{ Float::toInt() */
PHP_METHOD(Float, toInt)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  RETURN_LONG((long) self->data.floating.value);
}
/* }}} */

/* {{{ Float::toDouble() */
PHP_METHOD(Float, toDouble)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  RETURN_DOUBLE((double) self->data.floating.value);
}
/* }}} */

/* {{{ Float::min() */
PHP_METHOD(Float, min)
{
  php_driver_numeric *flt = NULL;
  object_init_ex(return_value, php_driver_float_ce);
  flt = PHP_DRIVER_GET_NUMERIC(return_value);
  flt->data.floating.value = FLT_MIN;
}
/* }}} */

/* {{{ Float::max() */
PHP_METHOD(Float, max)
{
  php_driver_numeric *flt = NULL;
  object_init_ex(return_value, php_driver_float_ce);
  flt = PHP_DRIVER_GET_NUMERIC(return_value);
  flt->data.floating.value = FLT_MAX;
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

static zend_function_entry php_driver_float_methods[] = {
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

static php_driver_value_handlers php_driver_float_handlers;

static HashTable *
php_driver_float_gc(php7to8_object *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_float_properties(php7to8_object *object TSRMLS_DC)
{
  php5to7_zval type;
  php5to7_zval value;

#if PHP_MAJOR_VERSION >= 8
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);
#else
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(object);
#endif
  HashTable         *props = zend_std_get_properties(object TSRMLS_CC);

  type = php_driver_type_scalar(CASS_VALUE_TYPE_FLOAT TSRMLS_CC);
  PHP5TO7_ZEND_HASH_UPDATE(props, "type", sizeof("type"), PHP5TO7_ZVAL_MAYBE_P(type), sizeof(zval));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  to_string(PHP5TO7_ZVAL_MAYBE_P(value), self TSRMLS_CC);
  PHP5TO7_ZEND_HASH_UPDATE(props, "value", sizeof("value"), PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval));

  return props;
}

static inline cass_int32_t
float_to_bits(cass_float_t value) {
  cass_int32_t bits;
  if (zend_isnan(value)) return 0x7fc00000; /* A canonical NaN value */
  memcpy(&bits, &value, sizeof(cass_int32_t));
  return bits;
}

static int
php_driver_float_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  cass_int32_t bits1, bits2;
  php_driver_numeric *flt1 = NULL;
  php_driver_numeric *flt2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  flt1 = PHP_DRIVER_GET_NUMERIC(obj1);
  flt2 = PHP_DRIVER_GET_NUMERIC(obj2);

  if (flt1->data.floating.value < flt2->data.floating.value) return -1;
  if (flt1->data.floating.value > flt2->data.floating.value) return  1;

  bits1 = float_to_bits(flt1->data.floating.value);
  bits2 = float_to_bits(flt2->data.floating.value);

  /* Handle NaNs and negative and positive 0.0 */
  return bits1 < bits2 ? -1 : bits1 > bits2;
}

static unsigned
php_driver_float_hash_value(zval *obj TSRMLS_DC)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(obj);
  return float_to_bits(self->data.floating.value);
}

static int
php_driver_float_cast(php7to8_object *object, zval *retval, int type TSRMLS_DC)
{
#if PHP_MAJOR_VERSION >= 8
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);
#else
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(object);
#endif

  switch (type) {
  case IS_LONG:
      ZVAL_LONG(retval, (long) self->data.floating.value);
      return SUCCESS;
  case IS_DOUBLE:
      ZVAL_DOUBLE(retval, (double) self->data.floating.value);
      return SUCCESS;
  case IS_STRING:
      return to_string(retval, self TSRMLS_CC);
  default:
     return FAILURE;
  }

  return SUCCESS;
}

static void
php_driver_float_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_float_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_numeric *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(numeric, ce);

  PHP5TO7_ZEND_OBJECT_INIT_EX(numeric, float, self, ce);
}

void php_driver_define_Float(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Float", php_driver_float_methods);
  php_driver_float_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_float_ce TSRMLS_CC, 2, php_driver_value_ce, php_driver_numeric_ce);
  php_driver_float_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_float_ce->create_object = php_driver_float_new;

  memcpy(&php_driver_float_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_float_handlers.std.get_properties  = php_driver_float_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_float_handlers.std.get_gc          = php_driver_float_gc;
#endif
  PHP7TO8_COMPARE(php_driver_float_handlers.std, php_driver_float_compare);
  php_driver_float_handlers.std.cast_object     = php_driver_float_cast;

  php_driver_float_handlers.hash_value = php_driver_float_hash_value;
  php_driver_float_handlers.std.clone_obj = NULL;
}
