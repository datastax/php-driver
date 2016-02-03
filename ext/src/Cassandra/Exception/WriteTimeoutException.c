#include "php_cassandra.h"

zend_class_entry *cassandra_write_timeout_exception_ce = NULL;

static zend_function_entry WriteTimeoutException_methods[] = {
  PHP_FE_END
};

void cassandra_define_WriteTimeoutException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\WriteTimeoutException", WriteTimeoutException_methods);
  cassandra_write_timeout_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_execution_exception_ce);
}
