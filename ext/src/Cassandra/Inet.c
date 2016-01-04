#include "php_cassandra.h"
#include "util/inet.h"

zend_class_entry *cassandra_inet_ce = NULL;

void
php_cassandra_inet_init(INTERNAL_FUNCTION_PARAMETERS)
{
  cassandra_inet *self;
  char *string;
  php5to7_size string_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_inet_ce TSRMLS_CC)) {
    self = PHP_CASSANDRA_GET_INET(getThis());
  } else {
    object_init_ex(return_value, cassandra_inet_ce);
    self = PHP_CASSANDRA_GET_INET(return_value);
  }

  if (!php_cassandra_parse_ip_address(string, &self->inet TSRMLS_CC)) {
    return;
  }
}

/* {{{ Cassandra\Inet::__construct(string) */
PHP_METHOD(Inet, __construct)
{
  php_cassandra_inet_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Cassandra\Inet::__toString() */
PHP_METHOD(Inet, __toString)
{
  cassandra_inet *inet = PHP_CASSANDRA_GET_INET(getThis());
  char *string;
  php_cassandra_format_address(inet->inet, &string);

  PHP5TO7_RETVAL_STRING(string);
  efree(string);
}
/* }}} */

/* {{{ Cassandra\Inet::address() */
PHP_METHOD(Inet, address)
{
  cassandra_inet *inet = PHP_CASSANDRA_GET_INET(getThis());
  char *string;
  php_cassandra_format_address(inet->inet, &string);

  PHP5TO7_RETVAL_STRING(string);
  efree(string);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, address)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_inet_methods[] = {
  PHP_ME(Inet, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Inet, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Inet, address, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_inet_handlers;

static HashTable *
php_cassandra_inet_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_cassandra_inet_properties(zval *object TSRMLS_DC)
{
  cassandra_inet *self = PHP_CASSANDRA_GET_INET(object);
  HashTable      *props = zend_std_get_properties(object TSRMLS_CC);
  php5to7_zval    address;

  char *string;
  php_cassandra_format_address(self->inet, &string);

  PHP5TO7_ZVAL_MAYBE_MAKE(address);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(address), string);
  efree(string);

  PHP5TO7_ZEND_HASH_UPDATE(props, "address", sizeof("address"), PHP5TO7_ZVAL_MAYBE_P(address), sizeof(zval));

  return props;
}

static int
php_cassandra_inet_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_inet *inet1 = NULL;
  cassandra_inet *inet2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  inet1 = PHP_CASSANDRA_GET_INET(obj1);
  inet2 = PHP_CASSANDRA_GET_INET(obj2);

  if (inet1->inet.address == inet2->inet.address)
    return 0;
  else if (inet1->inet.address < inet2->inet.address)
    return -1;
  else
    return 1;
}

static void
php_cassandra_inet_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_inet *self = PHP5TO7_ZEND_OBJECT_GET(inet, object);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_inet_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_inet *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(inet, ce);

  PHP5TO7_ZEND_OBJECT_INIT(inet, self, ce);
}

void cassandra_define_Inet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Inet", cassandra_inet_methods);
  cassandra_inet_ce = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_inet_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_inet_handlers.get_properties  = php_cassandra_inet_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_inet_handlers.get_gc          = php_cassandra_inet_gc;
#endif
  cassandra_inet_handlers.compare_objects = php_cassandra_inet_compare;
  cassandra_inet_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_inet_ce->create_object = php_cassandra_inet_new;
}
