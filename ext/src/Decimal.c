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
#include "util/hash.h"
#include "util/math.h"
#include "util/types.h"

#include <gmp.h>
#include <float.h>
#include <math.h>
#include <ext/spl/spl_exceptions.h>

zend_class_entry *php_driver_decimal_ce = NULL;

static void
to_mpf(mpf_t result, php_driver_numeric *decimal)
{
  mpf_t scale_factor;
  long scale;
  /* result = unscaled * pow(10, -scale) */
  mpf_set_z(result, decimal->data.decimal.value);

  scale = decimal->data.decimal.scale;
  mpf_init_set_si(scale_factor, 10);
  mpf_pow_ui(scale_factor, scale_factor, scale < 0 ? -scale : scale);

  if (scale > 0) {
    mpf_ui_div(scale_factor, 1, scale_factor);
  }

  mpf_mul(result, result, scale_factor);

  mpf_clear(scale_factor);
}

/*
 * IEEE 754 double precision floating point representation:
 *
 *  S   EEEEEEEEEEE  MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
 * [63][  62 - 52  ][                    51 - 0                          ]
 *
 * S = sign bit
 * E = exponent
 * M = mantissa
 */
#define DOUBLE_MANTISSA_BITS 52
#define DOUBLE_MANTISSA_MASK (cass_int64_t) ((1LL << DOUBLE_MANTISSA_BITS) - 1)
#define DOUBLE_EXPONENT_BITS 11
#define DOUBLE_EXPONENT_MASK (cass_int64_t) ((1LL << DOUBLE_EXPONENT_BITS) - 1)

static void
from_double(php_driver_numeric *result, double value)
{
  int denormal;
  char mantissa_str[32];
  cass_int64_t raw, mantissa, exponent;

  // Copy the bits of value into an int64 so that we can do bit manipulations on it.
  memcpy(&raw, &value, 8);

  mantissa = raw & DOUBLE_MANTISSA_MASK;
  exponent = (raw >> DOUBLE_MANTISSA_BITS) & DOUBLE_EXPONENT_MASK;

  /* This exponent is offset using 1023 unless it's a denormal value then its value
   * is the minimum value -1022
   */
  if (exponent == 0) {
    /* If the exponent is a zero then we have a denormal (subnormal) number. These are numbers
     * that represent small values around 0.0. The mantissa has the form of 0.xxxxxxxx...
     *
     * http://en.wikipedia.org/wiki/Denormal_number
     */
    denormal = 1;
    exponent = -1022;
  } else {
    /* Normal number The mantissa has the form of 1.xxxxxxx... */
    denormal = 0;
    exponent -= 1023;
  }

  /* Move the factional parts in the mantissa to the exponent. The significand
   * represents fractional parts:
   *
   * S = 1 + B51 * 2^-51 + B50 * 2^-52 ... + B0
   *
   */
  exponent -= DOUBLE_MANTISSA_BITS;

  if (!denormal) {
    /* Normal numbers have an implied one i.e. 1.xxxxxx... */
    mantissa |= (1LL << DOUBLE_MANTISSA_BITS);
  }

  /* Remove trailing zeros and move them to the exponent */
  while (exponent < 0 && (mantissa & 1) == 0) {
    ++exponent;
    mantissa >>= 1;
  }

  /* There isn't any "long long" setter method  */
#ifdef _WIN32
  sprintf(mantissa_str, "%I64d", mantissa);
#else
  sprintf(mantissa_str, "%lld", mantissa);
#endif
  mpz_set_str(result->data.decimal.value, mantissa_str, 10);

  /* Change the sign if negative */
  if (raw < 0) {
    mpz_neg(result->data.decimal.value, result->data.decimal.value);
  }

  if (exponent < 0) {
    /* Convert from pow(2, exponent) to pow(10, exponent):
     *
     * mantissa * pow(2, exponent) equals
     * mantissa * (pow(10, exponent) / pow(5, exponent))
     */
    mpz_t pow_5;
    mpz_init(pow_5);
    mpz_ui_pow_ui(pow_5, 5, -exponent);
    mpz_mul(result->data.decimal.value, result->data.decimal.value, pow_5);
    mpz_clear(pow_5);
    result->data.decimal.scale = -exponent;
  } else {
    mpz_mul_2exp(result->data.decimal.value, result->data.decimal.value, exponent);
    result->data.decimal.scale = 0;
  }
}

static int
to_double(zval* result, php_driver_numeric *decimal TSRMLS_DC)
{
  mpf_t value;
  mpf_init(value);
  to_mpf(value, decimal);

  if (mpf_cmp_d(value, -DBL_MAX) < 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    mpf_clear(value);
    return FAILURE;
  }

  if (mpf_cmp_d(value, DBL_MAX) > 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    mpf_clear(value);
    return FAILURE;
  }

  ZVAL_DOUBLE(result, mpf_get_d(value));
  mpf_clear(value);
  return SUCCESS;
}

static int
to_long(zval* result, php_driver_numeric *decimal TSRMLS_DC)
{
  mpf_t value;
  mpf_init(value);
  to_mpf(value, decimal);

  if (mpf_cmp_si(value, LONG_MIN) < 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    mpf_clear(value);
    return FAILURE;
  }

  if (mpf_cmp_si(value, LONG_MAX) > 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    mpf_clear(value);
    return FAILURE;
  }

  ZVAL_LONG(result, mpf_get_si(value));
  mpf_clear(value);
  return SUCCESS;
}

static int
to_string(zval* result, php_driver_numeric *decimal TSRMLS_DC)
{
  char* string;
  int string_len;
  php_driver_format_decimal(decimal->data.decimal.value, decimal->data.decimal.scale, &string, &string_len);

  PHP5TO7_ZVAL_STRINGL(result, string, string_len);
  efree(string);

  return SUCCESS;
}

static void
align_decimals(php_driver_numeric *lhs, php_driver_numeric *rhs)
{
  mpz_t pow_10;
  mpz_init(pow_10);
  if (lhs->data.decimal.scale < rhs->data.decimal.scale) {
    mpz_ui_pow_ui(pow_10, 10, rhs->data.decimal.scale - lhs->data.decimal.scale);
    mpz_mul(lhs->data.decimal.value, lhs->data.decimal.value, pow_10);
  } else if (lhs->data.decimal.scale > rhs->data.decimal.scale) {
    mpz_ui_pow_ui(pow_10, 10, lhs->data.decimal.scale - rhs->data.decimal.scale);
    mpz_mul(rhs->data.decimal.value, rhs->data.decimal.value, pow_10);
  }
  mpz_clear(pow_10);
}

void
php_driver_decimal_init(INTERNAL_FUNCTION_PARAMETERS)
{
  php_driver_numeric *self;
  zval* value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), php_driver_decimal_ce TSRMLS_CC)) {
    self = PHP_DRIVER_GET_NUMERIC(getThis());
  } else {
    object_init_ex(return_value, php_driver_decimal_ce);
    self = PHP_DRIVER_GET_NUMERIC(return_value);
  }

  if (Z_TYPE_P(value) == IS_LONG) {
    mpz_set_si(self->data.decimal.value, Z_LVAL_P(value));
    self->data.decimal.scale = 0;
  } else if (Z_TYPE_P(value) == IS_DOUBLE) {
    double val = Z_DVAL_P(value);
    if (zend_isnan(val) || zend_isinf(val)) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "Value of NaN or +/- infinity is not supported");
      return;
    }
    from_double(self, val);
  } else if (Z_TYPE_P(value) == IS_STRING) {
    if (!php_driver_parse_decimal(Z_STRVAL_P(value), Z_STRLEN_P(value),
                                     &self->data.decimal.value, &self->data.decimal.scale TSRMLS_CC)) {
      return;
    }
  } else if (Z_TYPE_P(value) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(value), php_driver_decimal_ce TSRMLS_CC)) {
    php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(value);
    mpz_set(self->data.decimal.value, decimal->data.decimal.value);
    self->data.decimal.scale = decimal->data.decimal.scale;
  } else {
    INVALID_ARGUMENT(value, "a long, a double, a numeric string or a " \
                            PHP_DRIVER_NAMESPACE "\\Decimal");
  }
}

/* {{{ Decimal::__construct(string) */
PHP_METHOD(Decimal, __construct)
{
  php_driver_decimal_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Decimal::__toString() */
PHP_METHOD(Decimal, __toString)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_string(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Decimal::type() */
PHP_METHOD(Decimal, type)
{
  php5to7_zval type = php_driver_type_scalar(CASS_VALUE_TYPE_DECIMAL TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(type), 1, 1);
}
/* }}} */

/* {{{ Decimal::value() */
PHP_METHOD(Decimal, value)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  char* string;
  int string_len;
  php_driver_format_integer(self->data.decimal.value, &string, &string_len);

  PHP5TO7_RETVAL_STRINGL(string, string_len);
  efree(string);
}
/* }}} */

PHP_METHOD(Decimal, scale)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  RETURN_LONG(self->data.decimal.scale);
}

/* {{{ Decimal::add() */
PHP_METHOD(Decimal, add)
{
  zval* num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_decimal_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_decimal_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    align_decimals(self, decimal);
    mpz_add(result->data.decimal.value, self->data.decimal.value, decimal->data.decimal.value);
    result->data.decimal.scale = MAX(self->data.decimal.scale, decimal->data.decimal.scale);
  } else {
    INVALID_ARGUMENT(num, "a " PHP_DRIVER_NAMESPACE "\\Decimal");
  }
}
/* }}} */

/* {{{ Decimal::sub() */
PHP_METHOD(Decimal, sub)
{
  zval* num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_decimal_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_decimal_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    align_decimals(self, decimal);
    mpz_sub(result->data.decimal.value, self->data.decimal.value, decimal->data.decimal.value);
    result->data.decimal.scale = MAX(self->data.decimal.scale, decimal->data.decimal.scale);
  } else {
    INVALID_ARGUMENT(num, "a " PHP_DRIVER_NAMESPACE "\\Decimal");
  }
}
/* }}} */

/* {{{ Decimal::mul() */
PHP_METHOD(Decimal, mul)
{
  zval* num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_decimal_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_decimal_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    mpz_mul(result->data.decimal.value, self->data.decimal.value, decimal->data.decimal.value);
    result->data.decimal.scale = self->data.decimal.scale + decimal->data.decimal.scale;
  } else {
    INVALID_ARGUMENT(num, "a " PHP_DRIVER_NAMESPACE "\\Decimal");
  }
}
/* }}} */

/* {{{ Decimal::div() */
PHP_METHOD(Decimal, div)
{
  /* TODO: Implementation of this a bit more difficult than anticipated. */
  zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC, "Not implemented");
}
/* }}} */

/* {{{ Decimal::mod() */
PHP_METHOD(Decimal, mod)
{
  /* TODO: We could implement a remainder method */
  zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC, "Not implemented");
}

/* {{{ Decimal::abs() */
PHP_METHOD(Decimal, abs)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  object_init_ex(return_value, php_driver_decimal_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);

  mpz_abs(result->data.decimal.value, self->data.decimal.value);
  result->data.decimal.scale = self->data.decimal.scale;
}
/* }}} */

/* {{{ Decimal::neg() */
PHP_METHOD(Decimal, neg)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  object_init_ex(return_value, php_driver_decimal_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);

  mpz_neg(result->data.decimal.value, self->data.decimal.value);
  result->data.decimal.scale = self->data.decimal.scale;
}
/* }}} */

/* {{{ Decimal::sqrt() */
PHP_METHOD(Decimal, sqrt)
{
  zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC, "Not implemented");
#if 0
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  mpf_t value;
  mpf_init(value);
  to_mpf(value, self);

  mpf_sqrt(value, value);

  mp_exp_t exponent;
  char* mantissa = mpf_get_str(NULL, &exponent, 10, 0, value);

  object_init_ex(return_value, php_driver_decimal_ce);
  php_driver_numeric *result = PHP_DRIVER_GET_NUMERIC(return_value);

  mpz_set_str(result->value.decimal_value, mantissa, 10);
  mp_bitcnt_t prec = mpf_get_prec(value);
  exponent -= prec;
  result->value.decimal_scale = -exponent;

  free(mantissa);
  mpf_clear(value);
#endif
}
/* }}} */

/* {{{ Decimal::toInt() */
PHP_METHOD(Decimal, toInt)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_long(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Decimal::toDouble() */
PHP_METHOD(Decimal, toDouble)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_double(return_value, self TSRMLS_CC);
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

static zend_function_entry php_driver_decimal_methods[] = {
  PHP_ME(Decimal, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, scale, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, mod, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, toInt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_decimal_handlers;

static HashTable*
php_driver_decimal_gc(php7to8_object *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_driver_decimal_properties(php7to8_object *object TSRMLS_DC)
{
  char* string;
  int string_len;
  php5to7_zval type;
  php5to7_zval value;
  php5to7_zval scale;

#if PHP_MAJOR_VERSION >= 8
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);
#else
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(object);
#endif
  HashTable         *props = zend_std_get_properties(object TSRMLS_CC);

  type = php_driver_type_scalar(CASS_VALUE_TYPE_DECIMAL TSRMLS_CC);
  PHP5TO7_ZEND_HASH_UPDATE(props, "type", sizeof("type"), PHP5TO7_ZVAL_MAYBE_P(type), sizeof(zval));

  php_driver_format_integer(self->data.decimal.value, &string, &string_len);
  PHP5TO7_ZVAL_MAYBE_MAKE(PHP5TO7_ZVAL_MAYBE_P(value));
  PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(value), string, string_len);
  efree(string);
  PHP5TO7_ZEND_HASH_UPDATE(props, "value", sizeof("value"), PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval));

  PHP5TO7_ZVAL_MAYBE_MAKE(scale);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(scale), self->data.decimal.scale);
  PHP5TO7_ZEND_HASH_UPDATE(props, "scale", sizeof("scale"), PHP5TO7_ZVAL_MAYBE_P(scale), sizeof(zval));

  return props;
}

static int
php_driver_decimal_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  php_driver_numeric *decimal1 = NULL;
  php_driver_numeric *decimal2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  decimal1 = PHP_DRIVER_GET_NUMERIC(obj1);
  decimal2 = PHP_DRIVER_GET_NUMERIC(obj2);

  if (decimal1->data.decimal.scale == decimal2->data.decimal.scale) {
    return mpz_cmp(decimal1->data.decimal.value, decimal2->data.decimal.value);
  } else if (decimal1->data.decimal.scale < decimal2->data.decimal.scale) {
    return -1;
  } else {
    return 1;
  }
}

static unsigned
php_driver_decimal_hash_value(zval *obj TSRMLS_DC)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(obj);
  return php_driver_mpz_hash((unsigned)self->data.decimal.scale, self->data.decimal.value);
}

static int
php_driver_decimal_cast(php7to8_object *object, zval *retval, int type TSRMLS_DC)
{
#if PHP_MAJOR_VERSION >= 8
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);
#else
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(object);
#endif

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
php_driver_decimal_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);

  mpz_clear(self->data.decimal.value);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_decimal_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_numeric *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(numeric, ce);

  self->type = PHP_DRIVER_DECIMAL;
  self->data.decimal.scale = 0;
  mpz_init(self->data.decimal.value);

  PHP5TO7_ZEND_OBJECT_INIT_EX(numeric, decimal, self, ce);
}

void php_driver_define_Decimal(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Decimal", php_driver_decimal_methods);
  php_driver_decimal_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_decimal_ce TSRMLS_CC, 2, php_driver_value_ce, php_driver_numeric_ce);
  php_driver_decimal_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_decimal_ce->create_object = php_driver_decimal_new;

  memcpy(&php_driver_decimal_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_decimal_handlers.std.get_properties  = php_driver_decimal_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_decimal_handlers.std.get_gc          = php_driver_decimal_gc;
#endif
  PHP7TO8_COMPARE(php_driver_decimal_handlers.std, php_driver_decimal_compare);
  php_driver_decimal_handlers.std.cast_object     = php_driver_decimal_cast;

  php_driver_decimal_handlers.hash_value = php_driver_decimal_hash_value;
  php_driver_decimal_handlers.std.clone_obj = NULL;
}
