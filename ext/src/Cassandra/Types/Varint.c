#include "php_cassandra.h"
#include "util/math.h"
#include <float.h>

extern zend_class_entry *cassandra_invalid_argument_exception_ce;

zend_class_entry *cassandra_varint_ce = NULL;

/* {{{ Cassandra\Types\Varint::__construct(string) */
PHP_METHOD(Varint, __construct)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if(Z_TYPE_P(num) == IS_LONG) {
    mpz_set_si(self->value, Z_LVAL_P(num));
  } else if(Z_TYPE_P(num) == IS_DOUBLE) {
    mpz_set_d(self->value, Z_DVAL_P(num));
  } else if(Z_TYPE_P(num) == IS_STRING) {
    if (!php_cassandra_parse_varint(Z_STRVAL_P(num), Z_STRLEN_P(num), &self->value TSRMLS_CC))
      return;
  } else if (Z_TYPE_P(num) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce)) {
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);
    mpz_set(self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "a long, a double, a numeric string or a Cassandra\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Varint::__toString() */
PHP_METHOD(Varint, __toString)
{
  cassandra_varint* number = (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

/* {{{ Cassandra\Types\Varint::value() */
PHP_METHOD(Varint, value)
{
  cassandra_varint* number = (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

/* {{{ Cassandra\Types\Varint::add() */
PHP_METHOD(Varint, add)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    cassandra_varint* result =
        (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_add(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Varint::sub() */
PHP_METHOD(Varint, sub)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    cassandra_varint* result =
        (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_sub(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Varint::mul() */
PHP_METHOD(Varint, mul)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    cassandra_varint* result =
        (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_mul(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Varint::div() */
PHP_METHOD(Varint, div)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    cassandra_varint* result =
        (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (mpz_sgn(varint->value) == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Divide by zero");
      return;
    }

    mpz_div(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Varint::mod() */
PHP_METHOD(Varint, mod)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    cassandra_varint* result =
        (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (mpz_sgn(varint->value) == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Modulo by zero");
      return;
    }

    mpz_mod(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Varint::abs() */
PHP_METHOD(Varint, abs)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_varint_ce);
  cassandra_varint* result =
      (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

  mpz_abs(result->value, self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Varint::neg() */
PHP_METHOD(Varint, neg)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_varint_ce);
  cassandra_varint* result =
      (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

  mpz_neg(result->value, self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Varint::sqrt() */
PHP_METHOD(Varint, sqrt)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (mpz_sgn(self->value) < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC,
                            "The operation would result in a complex number");
    return;
  }

  object_init_ex(return_value, cassandra_varint_ce);
  cassandra_varint* result =
      (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

  mpz_sqrt(result->value, self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Varint::toLong() */
PHP_METHOD(Varint, toLong)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (mpz_cmp_si(self->value, LONG_MIN) < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    return;
  }

  if (mpz_cmp_si(self->value, LONG_MAX) > 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    return;
  }

  RETURN_LONG(mpz_get_si(self->value));
}
/* }}} */

/* {{{ Cassandra\Types\Varint::toDouble() */
PHP_METHOD(Varint, toDouble)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (mpz_cmp_d(self->value, DBL_MIN) < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    return;
  }

  if (mpz_cmp_d(self->value, DBL_MAX) > 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    return;
  }

  RETURN_DOUBLE(mpz_get_d(self->value));
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

static zend_function_entry cassandra_varint_methods[] = {
  PHP_ME(Varint, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Varint, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, mod, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, toLong, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Varint, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_varint_handlers;

static HashTable*
php_cassandra_varint_properties(zval *object TSRMLS_DC)
{
  cassandra_varint* number = (cassandra_varint*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*        props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  int string_len;
  php_cassandra_format_integer(number->value, &string, &string_len);

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_varint_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_varint* varint1 = (cassandra_varint*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_varint* varint2 = (cassandra_varint*) zend_object_store_get_object(obj2 TSRMLS_CC);

  return mpz_cmp(varint1->value, varint2->value);
}

static int
php_cassandra_varint_cast(zval* obj, zval* retval, int type TSRMLS_DC)
{
  return FAILURE;
}

static void
php_cassandra_varint_free(void *object TSRMLS_DC)
{
  cassandra_varint* number = (cassandra_varint*) object;

  mpz_clear(number->value);
  zend_object_std_dtor(&number->zval TSRMLS_CC);

  efree(number);
}

static zend_object_value
php_cassandra_varint_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_varint *number;

  number = (cassandra_varint*) emalloc(sizeof(cassandra_varint));
  memset(number, 0, sizeof(cassandra_varint));

  number->type = CASSANDRA_VARINT;

  mpz_init(number->value);
  zend_object_std_init(&number->zval, class_type TSRMLS_CC);
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&number->zval, class_type);
#else
  zend_hash_copy(number->zval.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void*) NULL, sizeof(zval*));
#endif

  retval.handle   = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_varint_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_varint_handlers;

  return retval;
}

void cassandra_define_Varint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Types\\Varint", cassandra_varint_methods);
  cassandra_varint_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_varint_ce TSRMLS_CC, 1, cassandra_numeric_ce);
  cassandra_varint_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_varint_ce->create_object = php_cassandra_varint_new;

  memcpy(&cassandra_varint_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_varint_handlers.get_properties = php_cassandra_varint_properties;
  cassandra_varint_handlers.compare_objects = php_cassandra_varint_compare;
  cassandra_varint_handlers.cast_object = php_cassandra_varint_cast;
}
