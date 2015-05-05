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

static int
to_string(zval* result, cassandra_float* flt TSRMLS_DC)
{
  char* string;
  spprintf(&string, 0, "%.*F", (int) EG(precision), flt->value);
  ZVAL_STRING(result, string, 0);
  return SUCCESS;
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
             instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
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
  cassandra_float* self =
      (cassandra_float*) zend_object_store_get_object(getThis() TSRMLS_CC);

  to_string(return_value, self TSRMLS_CC);
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
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
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
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
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
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
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
      instanceof_function(Z_OBJCE_P(num), cassandra_float_ce TSRMLS_CC)) {
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

/* {{{ Cassandra\Types\Float::mod() */
PHP_METHOD(Float, mod)
{
  /* TODO: We could use fmod() here, but maybe we should add a remainder function
   * for floating point types.
   */
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
                          "Modulo not implemented for floating point types");
}

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

/* {{{ Cassandra\Types\Float::toInt() */
PHP_METHOD(Float, toInt)
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
  PHP_ME(Float, mod, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Float, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Float, toInt, arginfo_none, ZEND_ACC_PUBLIC)
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
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_float* flt1 = (cassandra_float*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_float* flt2 = (cassandra_float*) zend_object_store_get_object(obj2 TSRMLS_CC);

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

  self->type = CASSANDRA_FLOAT;

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&self->zval, class_type);
#else
  zend_hash_copy(self->zval.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void*) NULL, sizeof(zval*));
#endif

  retval.handle   = zend_objects_store_put(self, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_float_free, NULL TSRMLS_CC);
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