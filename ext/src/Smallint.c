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

#if !defined(HAVE_STDINT_H) && !defined(_MSC_STDINT_H_)
#  define INT16_MAX 32767
#  define INT16_MIN (-INT16_MAX-1)
#endif

zend_class_entry *php_driver_smallint_ce = NULL;

static int
to_double(zval *result, php_driver_numeric *smallint TSRMLS_DC)
{
  ZVAL_DOUBLE(result, (double) smallint->data.smallint.value);
  return SUCCESS;
}

static int
to_long(zval *result, php_driver_numeric *smallint TSRMLS_DC)
{
  ZVAL_LONG(result, (php5to7_long) smallint->data.smallint.value);
  return SUCCESS;
}

static int
to_string(zval *result, php_driver_numeric *smallint TSRMLS_DC)
{
  char *string;
  spprintf(&string, 0, "%d", smallint->data.smallint.value);
  PHP5TO7_ZVAL_STRING(result, string);
  efree(string);
  return SUCCESS;
}

void
php_driver_smallint_init(INTERNAL_FUNCTION_PARAMETERS)
{
  php_driver_numeric *self;
  zval *value;
  cass_int32_t number;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), php_driver_smallint_ce TSRMLS_CC)) {
    self = PHP_DRIVER_GET_NUMERIC(getThis());
  } else {
    object_init_ex(return_value, php_driver_smallint_ce);
    self = PHP_DRIVER_GET_NUMERIC(return_value);
  }

  if (Z_TYPE_P(value) == IS_OBJECT &&
           instanceof_function(Z_OBJCE_P(value), php_driver_smallint_ce TSRMLS_CC)) {
    php_driver_numeric *other = PHP_DRIVER_GET_NUMERIC(value);
    self->data.smallint.value = other->data.smallint.value;
  } else {
    if (Z_TYPE_P(value) == IS_LONG) {
      number = (cass_int32_t) Z_LVAL_P(value);

      if (number < INT16_MIN || number > INT16_MAX) {
        zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
          "value must be between -32768 and 32767, %ld given", Z_LVAL_P(value));
        return;
      }
    } else if (Z_TYPE_P(value) == IS_DOUBLE) {
      number = (cass_int32_t) Z_DVAL_P(value);

      if (number < INT16_MIN || number > INT16_MAX) {
        zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
          "value must be between -32768 and 32767, %g given", Z_DVAL_P(value));
        return;
      }
    } else if (Z_TYPE_P(value) == IS_STRING) {
      if (!php_driver_parse_int(Z_STRVAL_P(value), Z_STRLEN_P(value),
                                        &number TSRMLS_CC)) {

        // If the parsing function fails, it would have set an exception. If it's
        // a range error, the error message would be wrong because the parsing
        // function supports all 32-bit values, so the "valid" range it reports would
        // be too large for Smallint. Reset the exception in that case.

        if (errno == ERANGE) {
          zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
            "value must be between -32768 and 32767, %s given", Z_STRVAL_P(value));
        }
        return;
      }

      if (number < INT16_MIN || number > INT16_MAX) {
        zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
          "value must be between -32768 and 32767, %s given", Z_STRVAL_P(value));
        return;
      }
    } else {
      INVALID_ARGUMENT(value, "a long, a double, a numeric string or a " \
                              PHP_DRIVER_NAMESPACE "\\Smallint");
    }
    self->data.smallint.value = (cass_int16_t) number;
  }
}


/* {{{ Smallint::__construct(string) */
PHP_METHOD(Smallint, __construct)
{
  php_driver_smallint_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Smallint::__toString() */
PHP_METHOD(Smallint, __toString)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_string(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Smallint::type() */
PHP_METHOD(Smallint, type)
{
  php5to7_zval type = php_driver_type_scalar(CASS_VALUE_TYPE_SMALL_INT TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(type), 1, 1);
}
/* }}} */

/* {{{ Smallint::value() */
PHP_METHOD(Smallint, value)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_long(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Smallint::add() */
PHP_METHOD(Smallint, add)
{
  zval *addend;
  php_driver_numeric *self;
  php_driver_numeric *smallint;
  php_driver_numeric *result;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &addend) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(addend) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(addend), php_driver_smallint_ce TSRMLS_CC)) {
    self = PHP_DRIVER_GET_NUMERIC(getThis());
    smallint = PHP_DRIVER_GET_NUMERIC(addend);

    object_init_ex(return_value, php_driver_smallint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    result->data.smallint.value = self->data.smallint.value + smallint->data.smallint.value;
    if (result->data.smallint.value - smallint->data.smallint.value != self->data.smallint.value) {
      zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Sum is out of range");
      return;
    }
  } else {
    INVALID_ARGUMENT(addend, "a " PHP_DRIVER_NAMESPACE "\\Smallint");
  }
}
/* }}} */

/* {{{ Smallint::sub() */
PHP_METHOD(Smallint, sub)
{
  zval *difference;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &difference) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(difference) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(difference), php_driver_smallint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(difference);

    object_init_ex(return_value, php_driver_smallint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    result->data.smallint.value = self->data.smallint.value - smallint->data.smallint.value;
    if (result->data.smallint.value + smallint->data.smallint.value != self->data.smallint.value) {
      zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Difference is out of range");
      return;
    }
  } else {
    INVALID_ARGUMENT(difference, "a " PHP_DRIVER_NAMESPACE "\\Smallint");
  }
}
/* }}} */

/* {{{ Smallint::mul() */
PHP_METHOD(Smallint, mul)
{
  zval *multiplier;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &multiplier) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(multiplier) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(multiplier), php_driver_smallint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(multiplier);

    object_init_ex(return_value, php_driver_smallint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    result->data.smallint.value = self->data.smallint.value * smallint->data.smallint.value;
    if (smallint->data.smallint.value != 0 &&
        result->data.smallint.value / smallint->data.smallint.value != self->data.smallint.value) {
      zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Product is out of range");
      return;
    }
  } else {
    INVALID_ARGUMENT(multiplier, "a " PHP_DRIVER_NAMESPACE "\\Smallint");
  }
}
/* }}} */

/* {{{ Smallint::div() */
PHP_METHOD(Smallint, div)
{
  zval *divisor;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &divisor) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(divisor) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(divisor), php_driver_smallint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(divisor);

    object_init_ex(return_value, php_driver_smallint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    if (smallint->data.smallint.value == 0) {
      zend_throw_exception_ex(php_driver_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot divide by zero");
      return;
    }

    result->data.smallint.value = self->data.smallint.value / smallint->data.smallint.value;
  } else {
    INVALID_ARGUMENT(divisor, "a " PHP_DRIVER_NAMESPACE "\\Smallint");
  }
}
/* }}} */

/* {{{ Smallint::mod() */
PHP_METHOD(Smallint, mod)
{
  zval *divisor;
  php_driver_numeric *result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &divisor) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(divisor) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(divisor), php_driver_smallint_ce TSRMLS_CC)) {
    php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());
    php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(divisor);

    object_init_ex(return_value, php_driver_smallint_ce);
    result = PHP_DRIVER_GET_NUMERIC(return_value);

    if (smallint->data.smallint.value == 0) {
      zend_throw_exception_ex(php_driver_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot modulo by zero");
      return;
    }

    result->data.smallint.value = self->data.smallint.value % smallint->data.smallint.value;
  } else {
    INVALID_ARGUMENT(divisor, "a " PHP_DRIVER_NAMESPACE "\\Smallint");
  }
}
/* }}} */

/* {{{ Smallint::abs() */
PHP_METHOD(Smallint, abs)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  if (self->data.smallint.value == INT16_MIN) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value doesn't exist");
    return;
  }

  object_init_ex(return_value, php_driver_smallint_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);
  result->data.smallint.value = self->data.smallint.value < 0 ? -self->data.smallint.value : self->data.smallint.value;
}
/* }}} */

/* {{{ Smallint::neg() */
PHP_METHOD(Smallint, neg)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  if (self->data.smallint.value == INT16_MIN) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value doesn't exist");
    return;
  }

  object_init_ex(return_value, php_driver_smallint_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);
  result->data.smallint.value = -self->data.smallint.value;
}
/* }}} */

/* {{{ Smallint::sqrt() */
PHP_METHOD(Smallint, sqrt)
{
  php_driver_numeric *result = NULL;
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  if (self->data.smallint.value < 0) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
                            "Cannot take a square root of a negative number");
  }

  object_init_ex(return_value, php_driver_smallint_ce);
  result = PHP_DRIVER_GET_NUMERIC(return_value);
  result->data.smallint.value = (cass_int16_t) sqrt((long double) self->data.smallint.value);
}
/* }}} */

/* {{{ Smallint::toInt() */
PHP_METHOD(Smallint, toInt)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_long(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Smallint::toDouble() */
PHP_METHOD(Smallint, toDouble)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(getThis());

  to_double(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Smallint::min() */
PHP_METHOD(Smallint, min)
{
  php_driver_numeric *smallint = NULL;
  object_init_ex(return_value, php_driver_smallint_ce);
  smallint = PHP_DRIVER_GET_NUMERIC(return_value);
  smallint->data.smallint.value = INT16_MIN;
}
/* }}} */

/* {{{ Smallint::max() */
PHP_METHOD(Smallint, max)
{
  php_driver_numeric *smallint = NULL;
  object_init_ex(return_value, php_driver_smallint_ce);
  smallint = PHP_DRIVER_GET_NUMERIC(return_value);
  smallint->data.smallint.value = INT16_MAX;
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

static zend_function_entry php_driver_smallint_methods[] = {
  PHP_ME(Smallint, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, mod, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, toInt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Smallint, min, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_ME(Smallint, max, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_smallint_handlers;

static HashTable *
php_driver_smallint_gc(php7to8_object *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_smallint_properties(php7to8_object *object TSRMLS_DC)
{
  php5to7_zval type;
  php5to7_zval value;

#if PHP_MAJOR_VERSION >= 8
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);
#else
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(object);
#endif
  HashTable         *props = zend_std_get_properties(object TSRMLS_CC);

  type = php_driver_type_scalar(CASS_VALUE_TYPE_SMALL_INT TSRMLS_CC);
  PHP5TO7_ZEND_HASH_UPDATE(props, "type", sizeof("type"), PHP5TO7_ZVAL_MAYBE_P(type), sizeof(zval));

  PHP5TO7_ZVAL_MAYBE_MAKE(value);
  to_string(PHP5TO7_ZVAL_MAYBE_P(value), self TSRMLS_CC);
  PHP5TO7_ZEND_HASH_UPDATE(props, "value", sizeof("value"), PHP5TO7_ZVAL_MAYBE_P(value), sizeof(zval));

  return props;
}

static int
php_driver_smallint_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  php_driver_numeric *smallint1 = NULL;
  php_driver_numeric *smallint2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  smallint1 = PHP_DRIVER_GET_NUMERIC(obj1);
  smallint2 = PHP_DRIVER_GET_NUMERIC(obj2);

  if (smallint1->data.smallint.value == smallint2->data.smallint.value)
    return 0;
  else if (smallint1->data.smallint.value < smallint2->data.smallint.value)
    return -1;
  else
    return 1;
}

static unsigned
php_driver_smallint_hash_value(zval *obj TSRMLS_DC)
{
  php_driver_numeric *self = PHP_DRIVER_GET_NUMERIC(obj);
  return 31 * 17 + self->data.smallint.value;
}

static int
php_driver_smallint_cast(php7to8_object *object, zval *retval, int type TSRMLS_DC)
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
php_driver_smallint_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_numeric *self = PHP5TO7_ZEND_OBJECT_GET(numeric, object);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_smallint_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_numeric *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(numeric, ce);

  self->type = PHP_DRIVER_SMALLINT;

  PHP5TO7_ZEND_OBJECT_INIT_EX(numeric, smallint, self, ce);
}

void php_driver_define_Smallint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Smallint", php_driver_smallint_methods);
  php_driver_smallint_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_smallint_ce TSRMLS_CC, 2, php_driver_value_ce, php_driver_numeric_ce);
  php_driver_smallint_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_smallint_ce->create_object = php_driver_smallint_new;

  memcpy(&php_driver_smallint_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_smallint_handlers.std.get_properties  = php_driver_smallint_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_smallint_handlers.std.get_gc          = php_driver_smallint_gc;
#endif
  PHP7TO8_COMPARE(php_driver_smallint_handlers.std, php_driver_smallint_compare);
  php_driver_smallint_handlers.std.cast_object     = php_driver_smallint_cast;

  php_driver_smallint_handlers.hash_value = php_driver_smallint_hash_value;
}
