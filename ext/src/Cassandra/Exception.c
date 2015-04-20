#include "php_cassandra.h"

zend_class_entry *cassandra_exception_ce = NULL;

static zend_function_entry Exception_methods[] = {
  PHP_FE_END
};

void cassandra_define_Exception(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception", Exception_methods);
  cassandra_exception_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_exception_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
