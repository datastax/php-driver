#include "php_cassandra.h"

zend_class_entry *cassandra_default_session_ce = NULL;

PHP_METHOD(DefaultSession, execute)
{
  zval *statement;
  zval *options = NULL;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  cassandra_session* session = (cassandra_session*)
    zend_object_store_get_object(getThis() TSRMLS_CC);

  
}

PHP_METHOD(DefaultSession, executeAsync)
{

}

PHP_METHOD(DefaultSession, prepare)
{

}

PHP_METHOD(DefaultSession, prepareAsync)
{

}

PHP_METHOD(DefaultSession, close)
{

}

PHP_METHOD(DefaultSession, closeAsync)
{

}

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, statement, Cassandra\\Statement, 0)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_prepare, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, cql)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_default_session_methods[] = {
  PHP_ME(DefaultSession, execute, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, executeAsync, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, prepare, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, prepareAsync, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, close, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, closeAsync, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_default_session_handlers;

static HashTable*
php_cassandra_default_session_properties(zval *object TSRMLS_DC)
{
  cassandra_session* session = (cassandra_session*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*         props   = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_default_session_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_default_session_free(void *object TSRMLS_DC)
{
  cassandra_session* session = (cassandra_session*) object;

  zend_object_std_dtor(&session->zval TSRMLS_CC);

  if (!session->persist) {
    cass_session_free(session->session);
  }

  efree(session);
}

static zend_object_value
php_cassandra_default_session_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_session *session;

  session = (cassandra_session*) ecalloc(1, sizeof(cassandra_session));

  zend_object_std_init(&session->zval, class_type TSRMLS_CC);
  object_properties_init(&session->zval, class_type);

  session->session  = NULL;
  session->persist  = 0;

  retval.handle   = zend_objects_store_put(session,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_default_session_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_default_session_handlers;

  return retval;
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(DefaultSession)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\DefaultSession", cassandra_default_session_methods);
  cassandra_default_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_default_session_ce TSRMLS_CC, 1, cassandra_session_ce);
  cassandra_default_session_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_default_session_ce->create_object = php_cassandra_default_session_new;

  memcpy(&cassandra_default_session_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_default_session_handlers.get_properties  = php_cassandra_default_session_properties;
  cassandra_default_session_handlers.compare_objects = php_cassandra_default_session_compare;

  return SUCCESS;
}
