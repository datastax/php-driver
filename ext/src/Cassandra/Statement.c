#include "php_cassandra.h"

zend_class_entry *cassandra_statement_ce = NULL;

static zend_function_entry cassandra_statement_methods[] = {
  PHP_FE_END
};

void cassandra_define_Statement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Statement", cassandra_statement_methods);
  cassandra_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_statement_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
