#include "php_cassandra.h"
#include "util/math.h"

extern zend_class_entry* cassandra_invalid_argument_exception_ce;

zend_class_entry* cassandra_bigint_ce = NULL;

static int
ctype_digit(const char* s, int len)
{
  int i;
  for (i = 0; i < len; i++) {
    if (!isdigit(s[i]))
      return 0;
  }

  return 1;
}

/* {{{ Cassandra\Types\Bigint::__construct(string) */
PHP_METHOD(Bigint, __construct)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if(Z_TYPE_P(num) == IS_LONG) {
    self->value = (cass_int64_t) Z_LVAL_P(num);
  } else if(Z_TYPE_P(num) == IS_DOUBLE) {
    self->value = (cass_int64_t) Z_DVAL_P(num);
  } else if(Z_TYPE_P(num) == IS_STRING) {
    if (!php_cassandra_parse_bigint(Z_STRVAL_P(num), Z_STRLEN_P(num), &self->value TSRMLS_CC))
      return;
  } else if (Z_TYPE_P(num) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce)) {
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);
    self->value = bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a long, a double, a numeric string or a Cassandra\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::__toString() */
PHP_METHOD(Bigint, __toString)
{
  char* string;
  cassandra_bigint* number = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
#ifdef WIN32
  spprintf(&string, 0, "%I64d", (long long int) number->value);
#else
  spprintf(&string, 0, "%lld", (long long int) number->value);
#endif
  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::value() */
PHP_METHOD(Bigint, value)
{
  char* string;
  cassandra_bigint* number = (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
#ifdef WIN32
  spprintf(&string, 0, "%I64d", (long long int) number->value);
#else
  spprintf(&string, 0, "%lld", (long long int) number->value);
#endif
  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::add() */
PHP_METHOD(Bigint, add)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    cassandra_bigint* result =
        (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value + bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::sub() */
PHP_METHOD(Bigint, sub)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    cassandra_bigint* result =
        (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value - bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::mul() */
PHP_METHOD(Bigint, mul)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    cassandra_bigint* result =
        (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    result->value = self->value * bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::div() */
PHP_METHOD(Bigint, div)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    cassandra_bigint* result =
        (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (bigint->value == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Divide by zero");
      return;
    }

    result->value = self->value / bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::mod() */
PHP_METHOD(Bigint, mod)
{
  zval* num;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &num) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(num) == IS_OBJECT &&
      instanceof_function(Z_OBJCE_P(num), cassandra_bigint_ce)) {
    cassandra_bigint* self =
        (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);
    cassandra_bigint* bigint =
        (cassandra_bigint*) zend_object_store_get_object(num TSRMLS_CC);

    object_init_ex(return_value, cassandra_bigint_ce);
    cassandra_bigint* result =
        (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (bigint->value == 0) {
      zend_throw_exception_ex(cassandra_divide_by_zero_exception_ce, 0 TSRMLS_CC, "Modulo by zero");
      return;
    }

    result->value = self->value % bigint->value;
  } else {
    INVALID_ARGUMENT(num, "a Cassandra\\Types\\Bigint");
  }
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::abs() */
PHP_METHOD(Bigint, abs)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->value == LONG_LONG_MIN) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "No such value");
    return;
  }

  object_init_ex(return_value, cassandra_bigint_ce);
  cassandra_bigint* result =
      (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = self->value < 0 ? -self->value : self->value;
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::neg() */
PHP_METHOD(Bigint, neg)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_bigint_ce);
  cassandra_bigint* result =
      (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = -self->value;
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::sqrt() */
PHP_METHOD(Bigint, sqrt)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->value < 0) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC,
                            "The operation would result in a complex number");
  }

  object_init_ex(return_value, cassandra_bigint_ce);
  cassandra_bigint* result =
      (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  result->value = (cass_int64_t) sqrt((long double) self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::toLong() */
PHP_METHOD(Bigint, toLong)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->value < (cass_int64_t) LONG_MIN) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too small");
    return;
  }

  if (self->value > (cass_int64_t) LONG_MAX) {
    zend_throw_exception_ex(cassandra_range_exception_ce, 0 TSRMLS_CC, "Value is too big");
    return;
  }

  RETURN_LONG((long) self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::toDouble() */
PHP_METHOD(Bigint, toDouble)
{
  cassandra_bigint* self =
      (cassandra_bigint*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_DOUBLE((double) self->value);
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::minValue() */
PHP_METHOD(Bigint, minValue)
{
  object_init_ex(return_value, cassandra_bigint_ce);
  cassandra_bigint* bigint =
          (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  bigint->value = LONG_LONG_MIN;
}
/* }}} */

/* {{{ Cassandra\Types\Bigint::minValue() */
PHP_METHOD(Bigint, maxValue)
{
  object_init_ex(return_value, cassandra_bigint_ce);
  cassandra_bigint* bigint =
          (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
  bigint->value = LONG_LONG_MAX;
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

static zend_function_entry cassandra_bigint_methods[] = {
  PHP_ME(Bigint, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, value, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, add, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, sub, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, mul, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, div, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, mod, arginfo_num, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, abs, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, neg, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, sqrt, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, toLong, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, toDouble, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Bigint, minValue, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_ME(Bigint, maxValue, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_bigint_handlers;

static HashTable*
php_cassandra_bigint_properties(zval *object TSRMLS_DC)
{
  cassandra_bigint* number = (cassandra_bigint*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*        props  = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  int string_len;

#ifdef WIN32
  string_len = spprintf(&string, 0, "%I64d", (long long int) number->value);
#else
  string_len = spprintf(&string, 0, "%lld", (long long int) number->value);
#endif

  MAKE_STD_ZVAL(value);
  ZVAL_STRINGL(value, string, string_len, 0);

  zend_hash_update(props, "value", sizeof("value"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_bigint_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_bigint* bigint1 = NULL;
  cassandra_bigint* bigint2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  bigint1 = (cassandra_bigint*) zend_object_store_get_object(obj1 TSRMLS_CC);
  bigint2 = (cassandra_bigint*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (bigint1->value == bigint2->value)
    return 0;
  else if (bigint1->value < bigint2->value)
    return -1;
  else
    return 1;
}

static int
php_cassandra_bigint_cast(zval* obj, zval* retval, int type TSRMLS_DC)
{
  return FAILURE;
}

static void
php_cassandra_bigint_free(void *object TSRMLS_DC)
{
  cassandra_bigint* number = (cassandra_bigint*) object;

  zend_object_std_dtor(&number->zval TSRMLS_CC);

  efree(number);
}

static zend_object_value
php_cassandra_bigint_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_bigint *number;

  number = (cassandra_bigint*) emalloc(sizeof(cassandra_bigint));
  memset(number, 0, sizeof(cassandra_bigint));

  number->type = CASSANDRA_BIGINT;

  zend_object_std_init(&number->zval, class_type TSRMLS_CC);
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&number->zval, class_type);
#else
  zend_hash_copy(number->zval.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void*) NULL, sizeof(zval*));
#endif

  retval.handle   = zend_objects_store_put(number, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_bigint_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_bigint_handlers;

  return retval;
}

void cassandra_define_Bigint(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Types\\Bigint", cassandra_bigint_methods);
  cassandra_bigint_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_bigint_ce TSRMLS_CC, 1, cassandra_numeric_ce);
  cassandra_bigint_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_bigint_ce->create_object = php_cassandra_bigint_new;

  memcpy(&cassandra_bigint_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_bigint_handlers.get_properties = php_cassandra_bigint_properties;
  cassandra_bigint_handlers.compare_objects = php_cassandra_bigint_compare;
  cassandra_bigint_handlers.cast_object = php_cassandra_bigint_cast;
}
