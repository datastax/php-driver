#include "php_cassandra.h"
#include "util/math.h"
#include <float.h>
#include <math.h>

extern zend_class_entry *cassandra_invalid_argument_exception_ce;

zend_class_entry *cassandra_decimal_ce = NULL;

static void
calculate_decimal(mpf_t result, mpz_t unscaled, long scale)
{
  /* result = unscaled * pow(10, -scale) */
  mpf_set_z(result, unscaled);

  mpf_t scale_factor;
  mpf_init_set_si(scale_factor, 10);
  mpf_pow_ui(scale_factor, scale_factor, scale < 0 ? -scale : scale);

  if (scale > 0) {
    mpf_ui_div(scale_factor, 1, scale_factor);
  }

  mpf_mul(result, result, scale_factor);
  mpf_clear(scale_factor);
}

/* {{{ Cassandra\Types\Decimal::__construct(string) */
PHP_METHOD(Decimal, __construct)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  cassandra_decimal* self =
      (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if(Z_TYPE_P(num) == IS_LONG) {
    mpz_set_si(self->value, Z_LVAL_P(num));
    self->scale = 0;
  } else if(Z_TYPE_P(num) == IS_DOUBLE) {
    double value = Z_DVAL_P(num);
    if (zend_isnan(value) || zend_isinf(value)) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "Value is NaN or +/- infinity");
    }
    // TODO
  } else if(Z_TYPE_P(num) == IS_STRING) {
    int scale;
    if (!php_cassandra_parse_decimal(Z_STRVAL_P(num), Z_STRLEN_P(num),
                                     &self->value, &scale TSRMLS_CC))
      return;
    self->scale = scale;
  } else if (Z_TYPE_P(num) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(num), cassandra_decimal_ce)) {
    cassandra_decimal* decimal =
        (cassandra_decimal*) zend_object_store_get_object(num TSRMLS_CC);
    mpz_set(self->value, decimal->value);
    self->scale = decimal->scale;
  } else {
    INVALID_ARGUMENT(num, "a long, a double, a numeric string or a Cassandra\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::__toString() */
PHP_METHOD(Decimal, __toString)
{
  cassandra_decimal* number = (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_decimal(number->value, number->scale, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::value() */
PHP_METHOD(Decimal, value)
{
  cassandra_decimal* number = (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

PHP_METHOD(Decimal, scale)
{
  cassandra_decimal* number = (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_LONG(number->scale);
}

/* {{{ Cassandra\Types\Decimal::add() */
PHP_METHOD(Decimal, add)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_decimal_ce)) {
    cassandra_decimal* self =
        (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_decimal* decimal =
        (cassandra_decimal*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_decimal_ce);
    cassandra_decimal* result =
        (cassandra_decimal*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_add(result->value, self->value, decimal->value);
    result->scale = MAX(self->scale, decimal->scale);
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Decimal");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::sub() */
PHP_METHOD(Decimal, sub)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_decimal_ce)) {
    cassandra_decimal* self =
        (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_decimal* decimal =
        (cassandra_decimal*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_decimal_ce);
    cassandra_decimal* result =
        (cassandra_decimal*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_sub(result->value, self->value, decimal->value);
    result->scale = MAX(self->scale, decimal->scale);
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Decimal");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::mul() */
PHP_METHOD(Decimal, mul)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_decimal_ce)) {
    cassandra_decimal* self =
        (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_decimal* decimal =
        (cassandra_decimal*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_decimal_ce);
    cassandra_decimal* result =
        (cassandra_decimal*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_mul(result->value, self->value, decimal->value);
    result->scale = self->scale + decimal->scale;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Decimal");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::div() */
PHP_METHOD(Decimal, div)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_decimal_ce)) {
    cassandra_decimal* self =
        (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_decimal* decimal =
        (cassandra_decimal*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_decimal_ce);
    cassandra_decimal* result =
        (cassandra_decimal*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (mpz_sgn(self->value) == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Divide by zero");
      return;
    }

    mpz_div(result->value, self->value, decimal->value);
    result->scale = self->scale - decimal->scale;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Decimal");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::abs() */
PHP_METHOD(Decimal, abs)
{
  cassandra_decimal* self =
      (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_decimal_ce);
  cassandra_decimal* result =
      (cassandra_decimal*) zend_object_store_get_object(return_value TSRMLS_CC);

  mpz_abs(result->value, self->value);
  result->scale = self->scale;
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::neg() */
PHP_METHOD(Decimal, neg)
{
  cassandra_decimal* self =
      (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_decimal_ce);
  cassandra_decimal* result =
      (cassandra_decimal*) zend_object_store_get_object(return_value TSRMLS_CC);

  mpz_neg(result->value, self->value);
  result->scale = self->scale;
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::sqrt() */
PHP_METHOD(Decimal, sqrt)
{
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::toLong() */
PHP_METHOD(Decimal, toLong)
{
  cassandra_decimal* self =
      (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  mpf_t value;
  mpf_init(value);
  calculate_decimal(value, self->value, self->scale);

  if (mpf_cmp_si(value, LONG_MIN) < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    goto cleanup;
  }

  if (mpf_cmp_si(value, LONG_MAX) > 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    goto cleanup;
  }

  RETURN_LONG(mpf_get_si(value));

cleanup:
  mpf_clear(value);
}
/* }}} */

/* {{{ Cassandra\Types\Decimal::toDouble() */
PHP_METHOD(Decimal, toDouble)
{
  cassandra_decimal* self =
      (cassandra_decimal*) zend_object_store_get_object(getThis() TSRMLS_CC);

  mpf_t value;
  mpf_init(value);
  calculate_decimal(value, self->value, self->scale);

  if (mpf_cmp_d(value, DBL_MAX) < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    goto cleanup;
  }

  if (mpf_cmp_d(value, DBL_MIN) > 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    goto cleanup;
  }

  RETURN_DOUBLE(mpf_get_d(value));

cleanup:
  mpf_clear(value);
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

static zend_function_entry cassandra_decimal_methods[] = {
  PHP_ME(Decimal, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, scale, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, toLong, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Decimal, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_decimal_handlers;

static HashTable*
php_cassandra_decimal_properties(zval *object TSRMLS_DC)
{
  cassandra_decimal* number = (cassandra_decimal*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*         props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  zval* scale;
  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);
  MAKE_STD_ZVAL(scale);
  ZVAL_LONG(scale, number->scale);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);
  zend_hash_update(props, "scale", sizeof("scale"), &scale, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_decimal_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_decimal* decimal1 = (cassandra_decimal*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_decimal* decimal2 = (cassandra_decimal*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (decimal1->scale == decimal2->scale) {
    return mpz_cmp(decimal1->value, decimal2->value);
  } else if (decimal1->scale < decimal2->scale) {
    return -1;
  } else {
    return 1;
  }
}

static int
php_cassandra_decimal_cast(zval* obj, zval* retval, int type TSRMLS_DC)
{
  return FAILURE;
}

static void
php_cassandra_decimal_free(void *object TSRMLS_DC)
{
  cassandra_decimal* number = (cassandra_decimal*) object;

  mpz_clear(number->value);
  zend_object_std_dtor(&number->zval TSRMLS_CC);

  efree(number);
}

static zend_object_value
php_cassandra_decimal_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_decimal *number;

  number = (cassandra_decimal*) emalloc(sizeof(cassandra_decimal));
  memset(number, 0, sizeof(cassandra_decimal));

  number->type = CASSANDRA_DECIMAL;

  mpz_init(number->value);
  zend_object_std_init(&number->zval, class_type TSRMLS_CC);
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&number->zval, class_type);
#else
  zend_hash_copy(number->zval.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void*) NULL, sizeof(zval*));
#endif
  number->scale = 0;

  retval.handle   = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_decimal_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_decimal_handlers;

  return retval;
}

void cassandra_define_Decimal(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Types\\Decimal", cassandra_decimal_methods);
  cassandra_decimal_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_decimal_ce TSRMLS_CC, 1, cassandra_numeric_ce);
  cassandra_decimal_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_decimal_ce->create_object = php_cassandra_decimal_new;

  memcpy(&cassandra_decimal_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_decimal_handlers.get_properties = php_cassandra_decimal_properties;
  cassandra_decimal_handlers.compare_objects = php_cassandra_decimal_compare;
  cassandra_decimal_handlers.cast_object = php_cassandra_decimal_cast;
}
