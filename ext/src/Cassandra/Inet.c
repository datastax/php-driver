#include "php_cassandra.h"
#include "util/inet.h"

zend_class_entry *cassandra_inet_ce = NULL;

void
php_cassandra_inet_init(INTERNAL_FUNCTION_PARAMETERS)
{
  cassandra_inet* self;
  char* string;
  int string_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), cassandra_inet_ce TSRMLS_CC)) {
    self = (cassandra_inet*) zend_object_store_get_object(getThis() TSRMLS_CC);
  } else {
    object_init_ex(return_value, cassandra_inet_ce);
    self = (cassandra_inet*) zend_object_store_get_object(return_value TSRMLS_CC);
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
  cassandra_inet* self = (cassandra_inet*) zend_object_store_get_object(getThis() TSRMLS_CC);
  char* string;
  php_cassandra_format_address(self->inet, &string);

  RETURN_STRING(string, 0);
}
/* }}} */

/* {{{ Cassandra\Inet::type() */
PHP_METHOD(Inet, type)
{
  cassandra_inet* self = (cassandra_inet*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_ZVAL(self->type, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Inet::address() */
PHP_METHOD(Inet, address)
{
  cassandra_inet* self = (cassandra_inet*) zend_object_store_get_object(getThis() TSRMLS_CC);
  char* string;
  php_cassandra_format_address(self->inet, &string);

  RETURN_STRING(string, 0);
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
  PHP_ME(Inet, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Inet, address, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_inet_handlers;

static HashTable*
php_cassandra_inet_gc(zval *object, zval ***table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable*
php_cassandra_inet_properties(zval *object TSRMLS_DC)
{
  cassandra_inet* self  = (cassandra_inet*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*      props = zend_std_get_properties(object TSRMLS_CC);

  zval* value;
  char* string;
  php_cassandra_format_address(self->inet, &string);

  MAKE_STD_ZVAL(value);
  ZVAL_STRING(value, string, 0);

  zend_hash_update(props, "address", sizeof("address"), &value, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_inet_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_inet* inet1 = NULL;
  cassandra_inet* inet2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  inet1 = (cassandra_inet*) zend_object_store_get_object(obj1 TSRMLS_CC);
  inet2 = (cassandra_inet*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (inet1->inet.address == inet2->inet.address)
    return 0;
  else if (inet1->inet.address < inet2->inet.address)
    return -1;
  else
    return 1;
}

static void
php_cassandra_inet_free(void *object TSRMLS_DC)
{
  cassandra_inet* self = (cassandra_inet*) object;

  zval_ptr_dtor(&self->type);
  zend_object_std_dtor(&self->zval TSRMLS_CC);

  efree(self);
}

static zend_object_value
php_cassandra_inet_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_inet *inet;

  inet = (cassandra_inet*) emalloc(sizeof(cassandra_inet));
  memset(inet, 0, sizeof(cassandra_inet));

  zend_object_std_init(&inet->zval, class_type TSRMLS_CC);
  object_properties_init(&inet->zval, class_type);

  retval.handle   = zend_objects_store_put(inet, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_cassandra_inet_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_inet_handlers;

  return retval;
}

void cassandra_define_Inet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Inet", cassandra_inet_methods);
  cassandra_inet_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_inet_ce TSRMLS_CC, 1, cassandra_value_ce);
  memcpy(&cassandra_inet_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_inet_handlers.get_properties  = php_cassandra_inet_properties;
#if PHP_VERSION_ID >= 50400
  cassandra_inet_handlers.get_gc          = php_cassandra_inet_gc;
#endif
  cassandra_inet_handlers.compare_objects = php_cassandra_inet_compare;
  cassandra_inet_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_inet_ce->create_object = php_cassandra_inet_new;
}
