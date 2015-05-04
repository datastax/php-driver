#include "php_cassandra.h"
#include <math.h>

zend_class_entry* cassandra_float_ce = NULL;

static int
ctype_float(const char* s, int len)
{
  int i;
  int dot = -1;
  for (i = 0; i < len; i++) {
    if (s[i] == '.' && dot == -1)
      dot = i;

    if (!(isdigit(s[i]) || dot == i))
      return 0;
  }

  return 1;
}

/* {{{ Cassandra\Types\Float::__construct(string) */
PHP_METHOD(Float, __construct)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if(Z_TYPE_P(num) == IS_LONG) {
    self->value = (cass_float_t) Z_LVAL_P(num);
  } else if(Z_TYPE_P(num) == IS_DOUBLE) {
    self->value = (cass_float_t) Z_DVAL_P(num);
  } else if(Z_TYPE_P(num) == IS_STRING) {
    if (!ctype_float(Z_STRVAL_P(num), Z_STRLEN_P(num))) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "Invalid float value \"%s\"", Z_STRVAL_P(num));
      return;
    }
    self->value = (cass_float_t) strtof(Z_STRVAL_P(num), NULL);
  } else if (Z_TYPE_P(num) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(num), cassandra_float_ce)) {
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);
    self->value = flt->value;
  } else {
    INVALID_ARGUMENT(num, "a long, a double, a numeric string or a Cassandra\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Float::__toString() */
PHP_METHOD(Float, __toString)
{
  char* string;
  cassandra_float* self = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  spprintf(&string, 0, "%.*F", (int) EG(precision), self->value);
  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Types\Float::value() */
PHP_METHOD(Float, value)
{
  cassandra_float* self = (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_DOUBLE((double) self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Float::isInfinite() */
PHP_METHOD(Float, isInfinite)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_isinf(self->value));
}
/* }}} */

/* {{{ Cassandra\Types\Float::isFinite() */
PHP_METHOD(Float, isFinite)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_finite(self->value));
}
/* }}} */

/* {{{ Cassandra\Types\Float::isNaN() */
PHP_METHOD(Float, isNaN)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_isnan(self->value));
}
/* }}} */

/* {{{ Cassandra\Types\Float::add() */
PHP_METHOD(Float, add)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce)) {
    cassandra_float* self =
        (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_float_ce);
    cassandra_float* result =
        (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value + flt->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Float::sub() */
PHP_METHOD(Float, sub)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce)) {
    cassandra_float* self =
        (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_float_ce);
    cassandra_float* result =
        (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value - flt->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Float::mul() */
PHP_METHOD(Float, mul)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce)) {
    cassandra_float* self =
        (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_float_ce);
    cassandra_float* result =
        (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value * flt->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Float::div() */
PHP_METHOD(Float, div)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce)) {
    cassandra_float* self =
        (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_float* flt =
        (cassandra_float*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_float_ce);
    cassandra_float* result =
        (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (flt->value == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Divide by zero");
      return;
    }

    result->value = self->value / flt->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Float");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Float::abs() */
PHP_METHOD(Float, abs)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  object_init_ex(return_value, cassandra_float_ce);
  cassandra_float* result =
      (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = fabsf(self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Float::neg() */
PHP_METHOD(Float, neg)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  object_init_ex(return_value, cassandra_float_ce);
  cassandra_float* result =
      (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = -self->value;
}
/* }}} */

/* {{{ Cassandra\Types\Float::sqrt() */
PHP_METHOD(Float, sqrt)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->value < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC,
                            "The operation would result in a complex number");
  }

  object_init_ex(return_value, cassandra_float_ce);
  cassandra_float* result =
      (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = sqrtf(self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Float::toLong() */
PHP_METHOD(Float, toLong)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG((long) self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Float::toDouble() */
PHP_METHOD(Float, toDouble)
{
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_DOUBLE((double) self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Float::minValue() */
PHP_METHOD(Float, minValue)
{
  object_init_ex(return_value, cassandra_float_ce);
  cassandra_float* flt =
          (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  flt->value = LONG_LONG_MIN;
}
/* }}} */

/* {{{ Cassandra\Types\Float::minValue() */
PHP_METHOD(Float, maxValue)
{
  object_init_ex(return_value, cassandra_float_ce);
  cassandra_float* flt =
          (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
  flt->value = LONG_LONG_MAX;
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
  PHP_ME(Float, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, isInfinite, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, isFinite, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, isNaN, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, toLong, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, minValue, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_ME(Float, maxValue, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_float_handlers;

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
  cassandra_float* number1 = NULL;
  cassandra_float* number2 =  NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  number1 = (cassandra_float*) zend_object_store_get_object(obj1 TSRMLS_CC);
  number2 = (cassandra_float*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (number1->value == number2->value)
    return 0;
  else if (number1->value < number2->value)
    return -1;
  else
    return 1;
}

static int
php_cassandra_float_cast(zval* obj, zval* retval, int type TSRMLS_DC)
{
  return FAILURE;
}

static void
php_cassandra_float_free(void *object TSRMLS_DC)
{
  cassandra_float* self = (cassandra_float*) object;

  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_float_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_float *number;

  number = (cassandra_float*) emalloc(sizeof(cassandra_float));
  memset(number, 0, sizeof(cassandra_float));

  number->type = CASSANDRA_FLOAT;

  zend_object_std_init(&number->zval, class_type TSRMLS_CC);
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&number->zval, class_type);
#else
  zend_hash_copy(number->zval.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void*) NULL, sizeof(zval*));
#endif

  retval.handle   = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_float_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_float_handlers;

  return retval;
}

void cassandra_define_Float(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Types\\Float", cassandra_float_methods);
  cassandra_float_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_float_ce TSRMLS_CC, 1, cassandra_numeric_ce);
  cassandra_float_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_float_ce->create_object = php_cassandra_float_new;

  memcpy(&cassandra_float_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_float_handlers.get_properties = php_cassandra_float_properties;
  cassandra_float_handlers.compare_objects = php_cassandra_float_compare;
  cassandra_float_handlers.cast_object = php_cassandra_float_cast;
}
