#include "php_cassandra.h"

zend_class_entry *cassandra_read_timeout_exception_ce = NULL;

static zend_function_entry ReadTimeoutException_methods[] = {
  PHP_FE_END
};

void cassandra_define_ReadTimeoutException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ReadTimeoutException", ReadTimeoutException_methods);
  cassandra_read_timeout_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_execution_exception_ce);
}
