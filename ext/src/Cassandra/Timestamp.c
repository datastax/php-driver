#include "php_cassandra.h"
#include <ext/date/php_date.h>

zend_class_entry *cassandra_timestamp_ce = NULL;

void
php_cassandra_timestamp_init(INTERNAL_FUNCTION_PARAMETERS)
{
  cass_int64_t seconds = 0;
  cass_int64_t microseconds = 0;
  cassandra_timestamp *self;
  cass_int64_t value = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &seconds, &microseconds) == FAILURE) {
    return;
  }

  if (ZEND_NUM_ARGS() == 0) {
#ifdef WIN32
    seconds = (cass_int64_t) time(0);
#else
    struct timeval time;

    gettimeofday(&time, NULL);
    seconds = time.tv_sec;
    microseconds = (time.tv_usec / 1000) * 1000;
#endif
  }

  value += microseconds / 1000;
  value += (seconds * 1000);

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_timestamp_ce TSRMLS_CC)) {
    self = PHP_CASSANDRA_GET_TIMESTAMP(getThis());
  } else {
    object_init_ex(return_value, cassandra_timestamp_ce);
    self = PHP_CASSANDRA_GET_TIMESTAMP(return_value);
  }

  self->timestamp = value;
}

/* {{{ Cassandra\Timestamp::__construct(string) */
PHP_METHOD(Timestamp, __construct)
{
  php_cassandra_timestamp_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Timestamp::time */
PHP_METHOD(Timestamp, time)
{
  cassandra_timestamp *timestamp = PHP_CASSANDRA_GET_TIMESTAMP(getThis());

  RETURN_LONG(timestamp->timestamp / 1000);
}
/* }}} */

/* {{{ Cassandra\Timestamp::microtime(bool) */
PHP_METHOD(Timestamp, microtime)
{
  zend_bool get_as_float = 0;
  cassandra_timestamp *timestamp;
  char *ret = NULL;
  long sec = -1;
  double usec = 0.0f;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &get_as_float) == FAILURE) {
    return;
  }

  timestamp = PHP_CASSANDRA_GET_TIMESTAMP(getThis());

  if (get_as_float) {
    RETURN_DOUBLE((double) timestamp->timestamp / 1000.00);
  }

  sec    = (long) (timestamp->timestamp / 1000);
  usec   = (double) ((timestamp->timestamp - (sec * 1000)) / 1000.00);
  spprintf(&ret, 0, "%.8F %ld", usec, sec);
  PHP5TO7_RETVAL_STRING(ret);
  efree(ret);
}
/* }}} */

/* {{{ Cassandra\Timestamp::toDateTime() */
PHP_METHOD(Timestamp, toDateTime)
{
  cassandra_timestamp *timestamp;
  zval *datetime = NULL;
  php_date_obj *datetime_obj = NULL;
  char *str;
  int str_len;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  timestamp = PHP_CASSANDRA_GET_TIMESTAMP(getThis());

  PHP5TO7_ZVAL_MAYBE_MAKE(datetime);
  php_date_instantiate(php_date_get_date_ce(), datetime TSRMLS_CC);

#if PHP_MAJOR_VERSION >= 7
  datetime_obj = php_date_obj_from_obj(Z_OBJ_P(datetime));
#else
  datetime_obj = zend_object_store_get_object(datetime TSRMLS_CC);
#endif

  str_len      = spprintf(&str, 0, "@%ld", (long) (timestamp->timestamp / 1000));
  php_date_initialize(datetime_obj, str, str_len, NULL, NULL, 0 TSRMLS_CC);
  efree(str);

  RETVAL_ZVAL(datetime, 0, 0);
}
/* }}} */

/* {{{ Cassandra\Timestamp::__toString() */
PHP_METHOD(Timestamp, __toString)
{
  cassandra_timestamp *timestamp;
  char *ret = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  timestamp = PHP_CASSANDRA_GET_TIMESTAMP(getThis());

  spprintf(&ret, 0, "%lld", (long long int) timestamp->timestamp);
  PHP5TO7_RETVAL_STRING(ret);
  efree(ret);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, seconds)
  ZEND_ARG_INFO(0, microseconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_microtime, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, get_as_float)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_timestamp_methods[] = {
  PHP_ME(Timestamp, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, time, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, microtime, arginfo_microtime, ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, toDateTime, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_timestamp_handlers;

static HashTable *
php_cassandra_timestamp_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_cassandra_timestamp_properties(zval *object TSRMLS_DC)
{
  cassandra_timestamp *self = PHP_CASSANDRA_GET_TIMESTAMP(object);
  HashTable           *props = zend_std_get_properties(object TSRMLS_CC);
  php5to7_zval         seconds;
  php5to7_zval         microseconds;

  long sec  = (long) (self->timestamp / 1000);
  long usec = (long) ((self->timestamp - (sec * 1000)) * 1000);

  PHP5TO7_ZVAL_MAYBE_MAKE(seconds);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(seconds), sec);

  PHP5TO7_ZVAL_MAYBE_MAKE(microseconds);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(microseconds), usec);

  PHP5TO7_ZEND_HASH_UPDATE(props, "seconds", sizeof("seconds"), PHP5TO7_ZVAL_MAYBE_P(seconds), sizeof(zval));
  PHP5TO7_ZEND_HASH_UPDATE(props, "microseconds", sizeof("microseconds"), PHP5TO7_ZVAL_MAYBE_P(microseconds), sizeof(zval));

  return props;
}

static int
php_cassandra_timestamp_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_timestamp *timestamp1 = NULL;
  cassandra_timestamp *timestamp2 = NULL;
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  timestamp1 = PHP_CASSANDRA_GET_TIMESTAMP(obj1);
  timestamp2 = PHP_CASSANDRA_GET_TIMESTAMP(obj2);

  if (timestamp1->timestamp == timestamp2->timestamp)
    return 0;
  else if (timestamp1->timestamp < timestamp2->timestamp)
    return -1;
  else
    return 1;
}

static void
php_cassandra_timestamp_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_timestamp *self = PHP5TO7_ZEND_OBJECT_GET(timestamp, object);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_timestamp_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_timestamp *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(timestamp, ce);

  PHP5TO7_ZEND_OBJECT_INIT(timestamp, self, ce);
}

void cassandra_define_Timestamp(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Timestamp", cassandra_timestamp_methods);
  cassandra_timestamp_ce = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_timestamp_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_timestamp_handlers.get_properties  = php_cassandra_timestamp_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_timestamp_handlers.get_gc          = php_cassandra_timestamp_gc;
#endif
  cassandra_timestamp_handlers.compare_objects = php_cassandra_timestamp_compare;
  cassandra_timestamp_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_timestamp_ce->create_object = php_cassandra_timestamp_new;
}
