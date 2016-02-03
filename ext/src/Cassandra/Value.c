#include "php_cassandra.h"

zend_class_entry* cassandra_value_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_value_methods[] = {
  PHP_ABSTRACT_ME(Value, type, arginfo_none)
  PHP_FE_END
};

void
cassandra_define_Value(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Value", cassandra_value_methods);
  cassandra_value_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_value_ce->ce_flags |= ZEND_ACC_INTERFACE;
}

