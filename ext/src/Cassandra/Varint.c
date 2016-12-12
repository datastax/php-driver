/**
 * Copyright 2015-2016 DataStax, Inc.
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

#include <float.h>

zend_class_entry *php_driver_varint_ce = NULL;

static int
to_double(zval *result, php_driver_numeric *varint TSRMLS_DC)
{
  if (mpz_cmp_d(varint->varint_value, -DBL_MAX) < 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    return FAILURE;
  }

  if (mpz_cmp_d(varint->varint_value, DBL_MAX) > 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    return FAILURE;
  }

  ZVAL_DOUBLE(result, mpz_get_d(varint->varint_value));
  return SUCCESS;
}

static int
to_long(zval *result, php_driver_numeric *varint TSRMLS_DC)
{
  if (mpz_cmp_si(varint->varint_value, LONG_MIN) < 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    return FAILURE;
  }

  if (mpz_cmp_si(varint->varint_value, LONG_MAX) > 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    return FAILURE;
  }

  ZVAL_LONG(result, mpz_get_si(varint->varint_value));
  return SUCCESS;
}

static int
to_string(zval *result, php_driver_numeric *varint TSRMLS_DC)
{
  char *string;
  int string_len;
  php_driver_format_integer(varint->varint_value, &string, &string_len);

  PHP5TO7_ZVAL_STRINGL(result, string, string_len);
  efree(string);

  return SUCCESS;
}

void
php_driver_varint_init(INTERNAL_FUNCTION_PARAMETERS)
{
  zval *num;
  php_driver_numeric *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), php_driver_varint_ce TSRMLS_CC)) {
    self = PHP_DRIVER_GET_NUMERIC(getThis());
  } else {
    object_init_ex(return_value, php_driver_varint_ce);
    self = PHP_DRIVER_GET_NUMERIC(return_value);
  }

  if (Z_TYPE_P(num) == IS_LONG) {
    mpz_set_si(self->varint_value, Z_LVAL_P(num));
  } else if (Z_TYPE_P(num) == IS_DOUBLE) {
    mpz_set_d(self->varint_value, Z_DVAL_P(num));
  } else if (Z_TYPE_P(num) == IS_STRING) {
    php_driver_parse_varint(Z_STRVAL_P(num), Z_STRLEN_P(num), &self->varint_value TSRMLS_CC);
  } else if (Z_TYPE_P(num) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(num), php_driver_varint_ce TSRMLS_CC)) {
    php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(num);
    mpz_set(self->varint_value, varint->varint_value);
  } else {
    INVALID_ARGUMENT(num, "a long, double, numeric string or a " PHP_DRIVER_NAMESPACE "\\Varint instance");
  }
}

/* {{{ Varint::__construct(string) */
PHP_METHOD(Varint, __construct)
{
  php_driver_varint_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Varint::__toString() */
PHP_METHOD(Varint, __toString)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_string(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Varint::type() */
PHP_METHOD(Varint, type)
{
  php5to7_zval type = php_driver_type_scalar(CASS_VALUE_TYPE_VARINT TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(type), 1, 1);
}
/* }}} */

/* {{{ Varint::value() */
PHP_METHOD(Varint, value)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  char *string;
  int string_len;
  php_driver_format_integer(self->varint_value, &string, &string_len);

  PHP5TO7_RETVAL_STRINGL(string, string_len);
  efree(string);
}
/* }}} */

/* {{{ Varint::add() */
PHP_METHOD(Varint, add)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_varint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_varint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    mpz_add(result->varint_value, self->varint_value, varint->varint_value);
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Varint");
  }
}
/* }}} */

/* {{{ Varint::sub() */
PHP_METHOD(Varint, sub)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_varint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_varint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    mpz_sub(result->varint_value, self->varint_value, varint->varint_value);
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Varint");
  }
}
/* }}} */

/* {{{ Varint::mul() */
PHP_METHOD(Varint, mul)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_varint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_varint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    mpz_mul(result->varint_value, self->varint_value, varint->varint_value);
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Varint");
  }
}
/* }}} */

/* {{{ Varint::div() */
PHP_METHOD(Varint, div)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_varint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_varint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    if (mpz_sgn(varint->varint_value) == 0) {
      zend_throw_exception_ex(php_driver_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot divide by zero");
      return;
    }

    mpz_div(result->varint_value, self->varint_value, varint->varint_value);
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Varint");
  }
}
/* }}} */

/* {{{ Varint::mod() */
PHP_METHOD(Varint, mod)
{
  zval *num;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), php_driver_varint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(num);

    object_init_ex(return_value, php_driver_varint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    if (mpz_sgn(varint->varint_value) == 0) {
      zend_throw_exception_ex(php_driver_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot modulo by zero");
      return;
    }

    mpz_mod(result->varint_value, self->varint_value, varint->varint_value);
  } else {
    INVALID_ARGUMENT(num, "an instance of " PHP_DRIVER_NAMESPACE "\\Varint");
  }
}
/* }}} */

/* {{{ Varint::abs() */
PHP_METHOD(Varint, abs)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  object_init_ex(return_value, php_driver_varint_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);

  mpz_abs(result->varint_value, self->varint_value);
}
/* }}} */

/* {{{ Varint::neg() */
PHP_METHOD(Varint, neg)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  object_init_ex(return_value, php_driver_varint_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);

  mpz_neg(result->varint_value, self->varint_value);
}
/* }}} */

/* {{{ Varint::sqrt() */
PHP_METHOD(Varint, sqrt)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  if (mpz_sgn(self->varint_value) < 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
                            "Cannot take a square root of a negative number");
    return;
  }

  object_init_ex(return_value, php_driver_varint_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);

  mpz_sqrt(result->varint_value, self->varint_value);
}
/* }}} */

/* {{{ Varint::toInt() */
PHP_METHOD(Varint, toInt)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_long(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Varint::toDouble() */
PHP_METHOD(Varint, toDouble)
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

static zend_function_entry php_driver_varint_methods[] = {
  PHP_ME(Varint, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Varint, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, mod, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, toInt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_varint_handlers;

static HashTable *
php_driver_varint_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_varint_properties(zval *object TSRMLS_DC)
{
  char *string;
  int string_len;
  php5to7_zval type;
  php5to7_zval value;

  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(object);
  HashTable         *props = zend_std_get_properties(object TSRMLS_CC);

  php_driver_format_integer(self->varint_value, &string, &string_len);

  type = php_driver_type_scalar(CASS_VALUE_TYPE_VARINT TSRMLS_CC);
  PHP5TO7_ZEND_HASH_UPDATE(props, "type", sizeof("type"), PHP5TO7_ZVAL_MAYBE_P(type), sizeof(zval));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  PHP5TO7_ZVAL_STRINGL(PHP5TO7_ZVAL_MAYBE_P(value), string, string_len);
  efree(string);
  PHP5TO7_ZEND_HASH_UPDATE(props, "value", sizeof("value"), PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval));

  return props;
}

static int
php_driver_varint_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  php_driver_numeric *varint1 = NULL;
  php_driver_numeric *varint2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  varint1 = PHP_DRIVER_GET_NUMERIC(obj1);
  varint2 = PHP_DRIVER_GET_NUMERIC(obj2);

  return mpz_cmp(varint1->varint_value, varint2->varint_value);
}

static unsigned
php_driver_varint_hash_value(zval *obj TSRMLS_DC)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(obj);
  return php_driver_mpz_hash(0, self->varint_value);
}

static int
php_driver_varint_cast(zval *object, zval *retval, int type TSRMLS_DC)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(object);

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
php_driver_varint_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);

  mpz_clear(self->varint_value);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_varint_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_numeric *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(numeric, ce);

  mpz_init(self->varint_value);

  PHP5TO7_ZEND_OBJECT_INIT_EX(numeric, varint, self, ce);
}

void php_driver_define_Varint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Varint", php_driver_varint_methods);
  php_driver_varint_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_varint_ce TSRMLS_CC, 2, php_driver_value_ce, php_driver_numeric_ce);
  php_driver_varint_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_varint_ce->create_object = php_driver_varint_new;

  memcpy(&php_driver_varint_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_varint_handlers.std.get_properties  = php_driver_varint_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_varint_handlers.std.get_gc          = php_driver_varint_gc;
#endif
  php_driver_varint_handlers.std.compare_objects = php_driver_varint_compare;
  php_driver_varint_handlers.std.cast_object = php_driver_varint_cast;

  php_driver_varint_handlers.hash_value = php_driver_varint_hash_value;
  php_driver_varint_handlers.std.clone_obj = NULL;
}
