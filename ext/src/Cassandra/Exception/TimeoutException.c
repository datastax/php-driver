#include "php_cassandra.h"

extern zend_class_entry* cassandra_runtime_exception_ce;

zend_class_entry *cassandra_timeout_exception_ce = NULL;

static zend_function_entry TimeoutException_methods[] = {
  PHP_FE_END
};

void cassandra_define_TimeoutException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\TimeoutException", TimeoutException_methods);
  cassandra_timeout_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_runtime_exception_ce);
}
