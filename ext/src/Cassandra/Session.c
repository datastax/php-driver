#include "php_cassandra.h"

zend_class_entry *cassandra_session_ce = NULL;

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

static zend_function_entry cassandra_session_methods[] = {
  PHP_ABSTRACT_ME(Session, execute, arginfo_execute)
  PHP_ABSTRACT_ME(Session, executeAsync, arginfo_execute)
  PHP_ABSTRACT_ME(Session, prepare, arginfo_prepare)
  PHP_ABSTRACT_ME(Session, prepareAsync, arginfo_prepare)
  PHP_ABSTRACT_ME(Session, close, arginfo_timeout)
  PHP_ABSTRACT_ME(Session, closeAsync, arginfo_none)
  PHP_FE_END
};

void cassandra_define_Session(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Session", cassandra_session_methods);
  cassandra_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_session_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
