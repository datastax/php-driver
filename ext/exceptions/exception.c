#include "../php_cassandra.h"
#include "exception.h"

zend_class_entry *cassandra_ce_Exception = NULL;

static zend_function_entry CassandraException_methods[] = {
  PHP_FE_END
};

void cassandra_define_CassandraException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception", CassandraException_methods);
  cassandra_ce_Exception = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_Exception->ce_flags |= ZEND_ACC_INTERFACE;
}
