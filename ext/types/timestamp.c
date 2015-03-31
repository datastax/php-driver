#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "timestamp.h"
#include <ext/date/php_date.h>

extern zend_class_entry *cassandra_ce_InvalidArgumentException;
zend_class_entry *cassandra_ce_Timestamp = NULL;

/* {{{ Cassandra\Timestamp::__construct(string) */
PHP_METHOD(CassandraTimestamp, __construct)
{
  long seconds = 0;
  long microseconds = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &seconds, &microseconds) == FAILURE) {
    return;
  }

  if (ZEND_NUM_ARGS() == 0) {
#ifdef WIN32
    seconds = (long) time(0);
#else
    struct timeval time;

    gettimeofday(&time, NULL);
    seconds = time.tv_sec;
    microseconds = (time.tv_usec / 1000) * 1000;
#endif
  }

  cassandra_timestamp* timestamp;
  cass_int64_t value = 0;

  value += microseconds / 1000;
  value += (seconds * 1000);


  timestamp = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);
  timestamp->timestamp = value;
}
/* }}} */

/* {{{ Cassandra\Timestamp::time */
PHP_METHOD(CassandraTimestamp, time)
{
  cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_LONG(timestamp->timestamp / 1000);
}
/* }}} */

/* {{{ Cassandra\Timestamp::microtime(bool) */
PHP_METHOD(CassandraTimestamp, microtime)
{
  zend_bool get_as_float = 0;
  cassandra_timestamp* timestamp;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &get_as_float) == FAILURE) {
    return;
  }

  timestamp = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (get_as_float) {
    RETURN_DOUBLE((double) timestamp->timestamp / 1000.00);
  }

  char *ret;
  long sec    = (long) (timestamp->timestamp / 1000);
  double usec = (double) ((timestamp->timestamp - (sec * 1000)) / 1000.00);
  spprintf(&ret, 0, "%.8F %ld", usec, sec);
  RETURN_STRING(ret, 0);
}
/* }}} */

/* {{{ Cassandra\Timestamp::toDateTime() */
PHP_METHOD(CassandraTimestamp, toDateTime)
{
  cassandra_timestamp* timestamp;
  zval* datetime;
  php_date_obj* datetime_obj;
  char* str;
  int str_len;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  timestamp = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);

  MAKE_STD_ZVAL(datetime);
  php_date_instantiate(php_date_get_date_ce(), datetime TSRMLS_CC);

  datetime_obj = zend_object_store_get_object(datetime TSRMLS_CC);
  str_len      = spprintf(&str, 0, "@%ld", (long) (timestamp->timestamp / 1000));
  php_date_initialize(datetime_obj, str, str_len, NULL, NULL, 0 TSRMLS_CC);
  efree(str);

  RETVAL_ZVAL(datetime, 0, 0);
}
/* }}} */

/* {{{ Cassandra\Timestamp::__toString() */
PHP_METHOD(CassandraTimestamp, __toString)
{
  cassandra_timestamp* timestamp;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  timestamp = (cassandra_timestamp*) zend_object_store_get_object(getThis() TSRMLS_CC);

  char *ret;
  spprintf(&ret, 0, "%lld", timestamp->timestamp);
  RETURN_STRING(ret, 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, seconds)
  ZEND_ARG_INFO(0, microseconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_microtime, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, get_as_float)
ZEND_END_ARG_INFO()

static zend_function_entry CassandraTimestamp_methods[] = {
  PHP_ME(CassandraTimestamp, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimestamp, time, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimestamp, microtime, arginfo_microtime, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimestamp, toDateTime, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraTimestamp, __toString, NULL, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_timestamp_handlers;

static HashTable*
php_cassandra_timestamp_properties(zval *object TSRMLS_DC)
{
  cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*           props     = zend_std_get_properties(object TSRMLS_CC);

  zval* seconds;
  zval* microseconds;

  long sec  = (long) (timestamp->timestamp / 1000);
  long usec = (long) ((timestamp->timestamp - (sec * 1000)) * 1000);

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
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  cassandra_timestamp* timestamp1 = (cassandra_timestamp*) zend_object_store_get_object(obj1 TSRMLS_CC);
  cassandra_timestamp* timestamp2 = (cassandra_timestamp*) zend_object_store_get_object(obj2 TSRMLS_CC);

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
  cassandra_timestamp* timestamp = (cassandra_timestamp*) object;

  zend_object_std_dtor(&timestamp->zval TSRMLS_CC);

  efree(timestamp);
}

static zend_object_value
php_cassandra_timestamp_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_timestamp *timestamp;

  timestamp = (cassandra_timestamp*) emalloc(sizeof(cassandra_timestamp));
  memset(timestamp, 0, sizeof(cassandra_timestamp));

  zend_object_std_init(&timestamp->zval, class_type TSRMLS_CC);
  object_properties_init(&timestamp->zval, class_type TSRMLS_CC);

  retval.handle   = zend_objects_store_put(timestamp, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_timestamp_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_timestamp_handlers;

  return retval;
}

void cassandra_define_CassandraTimestamp(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Timestamp", CassandraTimestamp_methods);
  cassandra_ce_Timestamp = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_timestamp_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_timestamp_handlers.get_properties = php_cassandra_timestamp_properties;
  cassandra_timestamp_handlers.compare_objects = php_cassandra_timestamp_compare;
  cassandra_ce_Timestamp->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Timestamp->create_object = php_cassandra_timestamp_new;
}
