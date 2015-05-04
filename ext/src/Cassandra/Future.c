#include "php_cassandra.h"

zend_class_entry *cassandra_future_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_future_methods[] = {
  PHP_ABSTRACT_ME(Future, get, arginfo_timeout)
  PHP_FE_END
};

void cassandra_define_Future(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Future", cassandra_future_methods);
  cassandra_future_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_future_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
