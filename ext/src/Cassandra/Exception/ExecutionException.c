#include "php_cassandra.h"

zend_class_entry *cassandra_execution_exception_ce = NULL;

static zend_function_entry ExecutionException_methods[] = {
  PHP_FE_END
};

void cassandra_define_ExecutionException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ExecutionException", ExecutionException_methods);
  cassandra_execution_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_runtime_exception_ce);
}
