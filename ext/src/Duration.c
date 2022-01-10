#include "php_driver.h"
#include "php_driver_globals.h"
#include "php_driver_types.h"

#include "util/hash.h"
#include "util/math.h"
#include "util/types.h"

#include "Duration.h"

#if !defined(HAVE_STDINT_H) && !defined(_MSC_STDINT_H_)
#  define INT32_MAX 2147483647L
#  define INT32_MIN (-INT32_MAX-1)
#endif

zend_class_entry *php_driver_duration_ce = NULL;

static void to_string(zval *result, cass_int64_t value)
{
  char *string;
  spprintf(&string, 0, LL_FORMAT, value);
  PHP5TO7_ZVAL_STRING(result, string);
  efree(string);
}

static int get_param(zval* value,
                     const char* param_name,
                     cass_int64_t min,
                     cass_int64_t max,
                     cass_int64_t *destination  TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_LONG) {
    php5to7_long long_value = Z_LVAL_P(value);

    if (long_value > max || long_value < min) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "%s must be between " LL_FORMAT " and " LL_FORMAT ", " LL_FORMAT " given",
        param_name, min, max, long_value);
      return 0;
    }

    *destination = long_value;
  } else if (Z_TYPE_P(value) == IS_DOUBLE) {
    double double_value = Z_DVAL_P(value);

    if (double_value > max || double_value < min) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "%s must be between " LL_FORMAT " and " LL_FORMAT ", %g given",
        param_name, min, max, double_value);
      return 0;
    }
    *destination = (cass_int64_t) double_value;
  } else if (Z_TYPE_P(value) == IS_STRING) {
    cass_int64_t parsed_big_int;
    if (!php_driver_parse_bigint(Z_STRVAL_P(value), Z_STRLEN_P(value), &parsed_big_int TSRMLS_CC)) {
      return 0;
    }

    if (parsed_big_int > max || parsed_big_int < min) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "%s must be between " LL_FORMAT " and " LL_FORMAT ", " LL_FORMAT " given",
        param_name, min, max, parsed_big_int);
      return 0;
    }
    *destination = parsed_big_int;
  } else if (Z_TYPE_P(value) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(value), php_driver_bigint_ce TSRMLS_CC)) {
    php_driver_numeric *bigint = PHP_DRIVER_GET_NUMERIC(value);
    cass_int64_t bigint_value = bigint->data.bigint.value;

    if (bigint_value > max || bigint_value < min) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "%s must be between " LL_FORMAT " and " LL_FORMAT ", " LL_FORMAT " given",
        param_name, min, max, bigint_value);
      return 0;
    }

    *destination = bigint_value;
  } else {
    throw_invalid_argument(value, param_name, "a long, a double, a numeric string or a " \
                            PHP_DRIVER_NAMESPACE "\\Bigint" TSRMLS_CC);
    return 0;
  }
  return 1;
}

char *php_driver_duration_to_string(php_driver_duration *duration)
{
  // String representation of Duration is of the form -?MmoDdNns, for int M, D, N.
  // Negative durations lead with a minus sign. So (-3, -2, -1) results in
  // -3mo2d1ns.

  char* rep;
  int is_negative = 0;
  cass_int32_t final_months = duration->months;
  cass_int32_t final_days = duration->days;
  cass_int64_t final_nanos = duration->nanos;

  is_negative = final_months < 0 || final_days < 0 || final_nanos < 0;
  if (final_months < 0)
    final_months = -final_months;
  if (final_days < 0)
    final_days = -final_days;
  if (final_nanos < 0)
    final_nanos = -final_nanos;

  spprintf(&rep, 0, "%s%dmo%dd" LL_FORMAT "ns", is_negative ? "-" : "", final_months, final_days, final_nanos);
  return rep;
}

void
php_driver_duration_init(INTERNAL_FUNCTION_PARAMETERS)
{
  zval *months, *days, *nanos;
  cass_int64_t param;
  php_driver_duration *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &months, &days, &nanos) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_DURATION(getThis());

  if (!get_param(months, "months", INT32_MIN, INT32_MAX, &param  TSRMLS_CC)) {
    return;
  }
  self->months = (cass_int32_t)param;

  if (!get_param(days, "days", INT32_MIN, INT32_MAX, &param TSRMLS_CC)) {
    return;
  }
  self->days = (cass_int32_t)param;

  if (!get_param(nanos, "nanos", INT64_MIN, INT64_MAX, &self->nanos TSRMLS_CC)) {
    return;
  }

  // Verify that all three attributes are non-negative or non-positive.
  if (!(self->months <= 0 && self->days <= 0 && self->nanos <= 0) &&
      !(self->months >= 0 && self->days >= 0 && self->nanos >= 0)) {
    zend_throw_exception_ex(spl_ce_BadFunctionCallException, 0 TSRMLS_CC, "%s",
      "A duration must have all non-negative or non-positive attributes"
    );
  }
}

PHP_METHOD(Duration, __construct)
{
  php_driver_duration_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Duration, __toString)
{
  char* rep;
  php_driver_duration *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DURATION(getThis());

  // Build up string representation of this duration.
  rep = php_driver_duration_to_string(self);
  PHP5TO7_RETVAL_STRING(rep);
  efree(rep);
}

PHP_METHOD(Duration, type)
{
  php5to7_zval type = php_driver_type_scalar(CASS_VALUE_TYPE_DURATION TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(type), 1, 1);
}

PHP_METHOD(Duration, months)
{
  php_driver_duration *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DURATION(getThis());
  to_string(return_value, self->months);
}

PHP_METHOD(Duration, days)
{
  php_driver_duration *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DURATION(getThis());
  to_string(return_value, self->days);
}

PHP_METHOD(Duration, nanos)
{
  php_driver_duration *self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_DURATION(getThis());
  to_string(return_value, self->nanos);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 3)
  ZEND_ARG_INFO(0, months)
  ZEND_ARG_INFO(0, days)
  ZEND_ARG_INFO(0, nanos)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_duration_methods[] = {
  PHP_ME(Duration, __construct,  arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Duration, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Duration, months, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Duration, days, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Duration, nanos, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Duration, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_duration_handlers;

static HashTable *
php_driver_duration_properties(php7to8_object *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);
#if PHP_MAJOR_VERSION >= 8
  php_driver_duration  *self = PHP5TO7_ZEND_OBJECT_GET(duration, object);
#else
  php_driver_duration  *self = PHP_DRIVER_GET_DURATION(object);
#endif

  php5to7_zval wrapped_months, wrapped_days, wrapped_nanos;
  PHP5TO7_ZVAL_MAYBE_MAKE(wrapped_months);
  PHP5TO7_ZVAL_MAYBE_MAKE(wrapped_days);
  PHP5TO7_ZVAL_MAYBE_MAKE(wrapped_nanos);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(wrapped_months), self->months);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(wrapped_days), self->days);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(wrapped_nanos), self->nanos);
  PHP5TO7_ZEND_HASH_UPDATE(props, "months", sizeof("months"), PHP5TO7_ZVAL_MAYBE_P(wrapped_months), sizeof(zval));
  PHP5TO7_ZEND_HASH_UPDATE(props, "days", sizeof("days"), PHP5TO7_ZVAL_MAYBE_P(wrapped_days), sizeof(zval));
  PHP5TO7_ZEND_HASH_UPDATE(props, "nanos", sizeof("nanos"), PHP5TO7_ZVAL_MAYBE_P(wrapped_nanos), sizeof(zval));

  return props;
}

static int
php_driver_duration_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  php_driver_duration *left, *right;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  left = PHP_DRIVER_GET_DURATION(obj1);
  right = PHP_DRIVER_GET_DURATION(obj2);

  // Comparisons compare months, then days, then nanos.

  if (left->months < right->months)
    return -1;

  if (left->months > right->months)
    return 1;

  // months are the same; compare days.
  if (left->days < right->days)
    return -1;

  if (left->days > right->days)
    return 1;

  // days are the same; compare nanos.
  if (left->nanos < right->nanos)
    return -1;

  return (left->nanos == right->nanos) ? 0 : 1;
}

static unsigned
php_driver_duration_hash_value(zval *obj TSRMLS_DC)
{
  php_driver_duration *self = PHP_DRIVER_GET_DURATION(obj);
  unsigned hashv = 0;

  hashv = php_driver_combine_hash(hashv, (unsigned) self->months);
  hashv = php_driver_combine_hash(hashv, (unsigned) self->days);
  hashv = php_driver_combine_hash(hashv, (unsigned) self->nanos);

  return hashv;
}

static void
php_driver_duration_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_duration *self = PHP5TO7_ZEND_OBJECT_GET(duration, object);

  /* Clean up */

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_duration_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_duration *self = PHP5TO7_ZEND_OBJECT_ECALLOC(duration, ce);
  PHP5TO7_ZEND_OBJECT_INIT(duration, self, ce);
}

void php_driver_define_Duration(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Duration", php_driver_duration_methods);
  php_driver_duration_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_duration_ce TSRMLS_CC, 1, php_driver_value_ce);

  php_driver_duration_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_duration_ce->create_object = php_driver_duration_new;

  memcpy(&php_driver_duration_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_duration_handlers.std.get_properties  = php_driver_duration_properties;
  PHP7TO8_COMPARE(php_driver_duration_handlers.std, php_driver_duration_compare);

  php_driver_duration_handlers.hash_value = php_driver_duration_hash_value;
  php_driver_duration_handlers.std.clone_obj = NULL;
}
