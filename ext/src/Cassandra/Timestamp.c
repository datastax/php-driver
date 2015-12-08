#include "php_cassandra.h"
#include <ext/date/php_date.h>

zend_class_entry *cassandra_timestamp_ce = NULL;

void
php_cassandra_timestamp_init(INTERNAL_FUNCTION_PARAMETERS)
{
  cass_int64_t seconds = 0;
  cass_int64_t microseconds = 0;
  cassandra_timestamp* self;
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
    self = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);
  } else {
    object_init_ex(return_value, cassandra_timestamp_ce);
    self = (cassandra_timestamp*) zend_object_store_get_object(return_value TSRMLS_CC);
  }

  self->timestamp = value;
}

/* {{{ Cassandra\Timestamp::__construct(string) */
PHP_METHOD(Timestamp, __construct)
{
  php_cassandra_timestamp_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Timestamp::type() */
PHP_METHOD(Timestamp, type)
{
  cassandra_timestamp* self = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->type, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Timestamp::time */
PHP_METHOD(Timestamp, time)
{
  cassandra_timestamp* self = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(self->timestamp / 1000);
}
/* }}} */

/* {{{ Cassandra\Timestamp::microtime(bool) */
PHP_METHOD(Timestamp, microtime)
{
  zend_bool get_as_float = 0;
  cassandra_timestamp* self;
  char *ret = NULL;
  long sec = -1;
  double usec = 0.0f;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &get_as_float) == FAILURE) {
    return;
  }

  self = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (get_as_float) {
    RETURN_DOUBLE((double) self->timestamp / 1000.00);
  }

  sec    = (long) (self->timestamp / 1000);
  usec   = (double) ((self->timestamp - (sec * 1000)) / 1000.00);
  spprintf(&ret, 0, "%.8F %ld", usec, sec);
  RETURN_STRING(ret, 0);
}
/* }}} */

/* {{{ Cassandra\Timestamp::toDateTime() */
PHP_METHOD(Timestamp, toDateTime)
{
  cassandra_timestamp* self;
  zval* datetime;
  php_date_obj* datetime_obj;
  char* str;
  int str_len;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);

  MAKE_STD_ZVAL(datetime);
  php_date_instantiate(php_date_get_date_ce(), datetime TSRMLS_CC);

  datetime_obj = zend_object_store_get_object(datetime TSRMLS_CC);
  str_len      = spprintf(&str, 0, "@%ld", (long) (self->timestamp / 1000));
  php_date_initialize(datetime_obj, str, str_len, NULL, NULL, 0 TSRMLS_CC);
  efree(str);

  RETVAL_ZVAL(datetime, 0, 0);
}
/* }}} */

/* {{{ Cassandra\Timestamp::__toString() */
PHP_METHOD(Timestamp, __toString)
{
  cassandra_timestamp* self;
  char *ret = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);

  spprintf(&ret, 0, "%lld", (long long int) self->timestamp);
  RETURN_STRING(ret, 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, seconds)
  ZEND_ARG_INFO(0, microseconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_microtime, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, get_as_float)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_timestamp_methods[] = {
  PHP_ME(Timestamp, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, time, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, microtime, arginfo_microtime, ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, toDateTime, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Timestamp, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_timestamp_handlers;

static HashTable*
php_cassandra_timestamp_gc(zval *object, zval ***table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_timestamp_properties(zval *object TSRMLS_DC)
{
  cassandra_timestamp* self  = (cassandra_timestamp*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*           props = zend_std_get_properties(object TSRMLS_CC);

  zval* seconds;
  zval* microseconds;

  long sec  = (long) (self->timestamp / 1000);
  long usec = (long) ((self->timestamp - (sec * 1000)) * 1000);

  MAKE_STD_ZVAL(seconds);
  ZVAL_LONG(seconds, sec);
  MAKE_STD_ZVAL(microseconds);
  ZVAL_LONG(microseconds, usec);

  zend_hash_update(props, "seconds", sizeof("seconds"), &seconds, sizeof(zval), NULL);
  zend_hash_update(props, "microseconds", sizeof("microseconds"), &microseconds, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_timestamp_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_timestamp* timestamp1 = NULL;
  cassandra_timestamp* timestamp2 = NULL;
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  timestamp1 = (cassandra_timestamp*) zend_object_store_get_object(obj1 TSRMLS_CC);
  timestamp2 = (cassandra_timestamp*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (timestamp1->timestamp == timestamp2->timestamp)
    return 0;
  else if (timestamp1->timestamp < timestamp2->timestamp)
    return -1;
  else
    return 1;
}

static void
php_cassandra_timestamp_free(void *object TSRMLS_DC)
{
  cassandra_timestamp* self = (cassandra_timestamp*) object;

  zval_ptr_dtor(&self->type);
  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_timestamp_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_timestamp *timestamp;

  timestamp = (cassandra_timestamp*) emalloc(sizeof(cassandra_timestamp));
  memset(timestamp, 0, sizeof(cassandra_timestamp));

  zend_object_std_init(&timestamp->zval, class_type TSRMLS_CC);
  object_properties_init(&timestamp->zval, class_type);

  retval.handle   = zend_objects_store_put(timestamp, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_timestamp_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_timestamp_handlers;

  return retval;
}

void cassandra_define_Timestamp(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Timestamp", cassandra_timestamp_methods);
  cassandra_timestamp_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_timestamp_ce TSRMLS_CC, 1, cassandra_value_ce);
  memcpy(&cassandra_timestamp_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_timestamp_handlers.get_properties  = php_cassandra_timestamp_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_timestamp_handlers.get_gc          = php_cassandra_timestamp_gc;
#endif
  cassandra_timestamp_handlers.compare_objects = php_cassandra_timestamp_compare;
  cassandra_timestamp_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_timestamp_ce->create_object = php_cassandra_timestamp_new;
}
