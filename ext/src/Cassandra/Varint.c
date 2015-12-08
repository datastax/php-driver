#include "php_cassandra.h"
#include "util/math.h"
#include "util/types.h"
#include <float.h>

zend_class_entry *cassandra_varint_ce = NULL;

static int
to_double(zval* result, cassandra_varint* varint TSRMLS_DC)
{
  if (mpz_cmp_d(varint->value, -DBL_MAX) < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    return FAILURE;
  }

  if (mpz_cmp_d(varint->value, DBL_MAX) > 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    return FAILURE;
  }

  ZVAL_DOUBLE(result, mpz_get_d(varint->value));
  return SUCCESS;
}

static int
to_long(zval* result, cassandra_varint* varint TSRMLS_DC)
{
  if (mpz_cmp_si(varint->value, LONG_MIN) < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    return FAILURE;
  }

  if (mpz_cmp_si(varint->value, LONG_MAX) > 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    return FAILURE;
  }

  ZVAL_LONG(result, mpz_get_si(varint->value));
  return SUCCESS;
}

static int
to_string(zval* result, cassandra_varint* varint TSRMLS_DC)
{
  char* string;
  int string_len;
  php_cassandra_format_integer(varint->value, &string, &string_len);

  ZVAL_STRINGL(result, string, string_len, 0);
  return SUCCESS;
}

void
php_cassandra_varint_init(INTERNAL_FUNCTION_PARAMETERS)
{
  zval* num;
  cassandra_varint* self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_varint_ce TSRMLS_CC)) {
    self = (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
  } else {
    object_init_ex(return_value, cassandra_varint_ce);
    self = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);
  }

  if (Z_TYPE_P(num) == IS_LONG) {
    mpz_set_si(self->value, Z_LVAL_P(num));
  } else if (Z_TYPE_P(num) == IS_DOUBLE) {
    mpz_set_d(self->value, Z_DVAL_P(num));
  } else if (Z_TYPE_P(num) == IS_STRING) {
    php_cassandra_parse_varint(Z_STRVAL_P(num), Z_STRLEN_P(num), &self->value TSRMLS_CC);
  } else if (Z_TYPE_P(num) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce TSRMLS_CC)) {
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);
    mpz_set(self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "a long, double, numeric string or a Cassandra\\Varint instance");
  }
}

/* {{{ Cassandra\Varint::__construct(string) */
PHP_METHOD(Varint, __construct)
{
  php_cassandra_varint_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Varint::__toString() */
PHP_METHOD(Varint, __toString)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  to_string(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Varint::type() */
PHP_METHOD(Varint, type)
{
  cassandra_varint* self = (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->type, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Varint::value() */
PHP_METHOD(Varint, value)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char* string;
  int string_len;
  php_cassandra_format_integer(self->value, &string, &string_len);

  RETURN_STRINGL(string, string_len, 0);
}
/* }}} */

/* {{{ Cassandra\Varint::add() */
PHP_METHOD(Varint, add)
{
  zval* num;
  cassandra_varint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce TSRMLS_CC)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    result = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_add(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Varint::sub() */
PHP_METHOD(Varint, sub)
{
  zval* num;
  cassandra_varint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce TSRMLS_CC)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    result = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_sub(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Varint::mul() */
PHP_METHOD(Varint, mul)
{
  zval* num;
  cassandra_varint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce TSRMLS_CC)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    result = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    mpz_mul(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Varint::div() */
PHP_METHOD(Varint, div)
{
  zval* num;
  cassandra_varint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce TSRMLS_CC)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    result = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (mpz_sgn(varint->value) == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot divide by zero");
      return;
    }

    mpz_div(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Varint::mod() */
PHP_METHOD(Varint, mod)
{
  zval* num;
  cassandra_varint* result = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_varint_ce TSRMLS_CC)) {
    cassandra_varint* self =
        (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_varint* varint =
        (cassandra_varint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_varint_ce);
    result = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (mpz_sgn(varint->value) == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Cannot modulo by zero");
      return;
    }

    mpz_mod(result->value, self->value, varint->value);
  } else {
    INVALID_ARGUMENT(num, "an instance of Cassandra\\Varint");
  }
}
/* }}} */

/* {{{ Cassandra\Varint::abs() */
PHP_METHOD(Varint, abs)
{
  cassandra_varint* result = NULL;

  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_varint_ce);
  result = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

  mpz_abs(result->value, self->value);
}
/* }}} */

/* {{{ Cassandra\Varint::neg() */
PHP_METHOD(Varint, neg)
{
  cassandra_varint* result = NULL;

  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_varint_ce);
  result = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

  mpz_neg(result->value, self->value);
}
/* }}} */

/* {{{ Cassandra\Varint::sqrt() */
PHP_METHOD(Varint, sqrt)
{
  cassandra_varint* result = NULL;

  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (mpz_sgn(self->value) < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC,
                            "Cannot take a square root of a negative number");
    return;
  }

  object_init_ex(return_value, cassandra_varint_ce);
  result = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);

  mpz_sqrt(result->value, self->value);
}
/* }}} */

/* {{{ Cassandra\Varint::toInt() */
PHP_METHOD(Varint, toInt)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  to_long(return_value, self TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Varint::toDouble() */
PHP_METHOD(Varint, toDouble)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(getThis() TSRMLS_CC);

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

static zend_function_entry cassandra_varint_methods[] = {
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

static zend_object_handlers cassandra_varint_handlers;

static HashTable*
php_cassandra_varint_gc(zval *object, zval ***table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_varint_properties(zval *object TSRMLS_DC)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*        props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  int string_len;
  php_cassandra_format_integer(self->value, &string, &string_len);

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_varint_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_varint* varint1 = NULL;
  cassandra_varint* varint2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  varint1 = (cassandra_varint*) zend_object_store_get_object(obj1 TSRMLS_CC);
  varint2 = (cassandra_varint*) zend_object_store_get_object(obj2 TSRMLS_CC);

  return mpz_cmp(varint1->value, varint2->value);
}

static int
php_cassandra_varint_cast(zval* object, zval* retval, int type TSRMLS_DC)
{
  cassandra_varint* self =
      (cassandra_varint*) zend_object_store_get_object(object TSRMLS_CC);

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
php_cassandra_varint_free(void *object TSRMLS_DC)
{
  cassandra_varint* self = (cassandra_varint*) object;

  mpz_clear(self->value);

  zval_ptr_dtor(&self->type);
  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_varint_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_varint *self;

  self = (cassandra_varint*) emalloc(sizeof(cassandra_varint));
  memset(self, 0, sizeof(cassandra_varint));

  self->type = php_cassandra_type_scalar(CASS_VALUE_TYPE_VARINT TSRMLS_CC);
  Z_ADDREF_P(self->type);

  mpz_init(self->value);
  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  retval.handle   = zend_objects_store_put(self, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_varint_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_varint_handlers;

  return retval;
}

void cassandra_define_Varint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Varint", cassandra_varint_methods);
  cassandra_varint_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_varint_ce TSRMLS_CC, 1, cassandra_numeric_ce);
  cassandra_varint_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_varint_ce->create_object = php_cassandra_varint_new;

  memcpy(&cassandra_varint_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_varint_handlers.get_properties  = php_cassandra_varint_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_varint_handlers.get_gc          = php_cassandra_varint_gc;
#endif
  cassandra_varint_handlers.compare_objects = php_cassandra_varint_compare;
  cassandra_varint_handlers.cast_object = php_cassandra_varint_cast;
}
