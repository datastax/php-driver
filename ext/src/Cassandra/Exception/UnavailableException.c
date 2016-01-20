#include "php_cassandra.h"

zend_class_entry *cassandra_unavailable_exception_ce = NULL;

static zend_function_entry UnavailableException_methods[] = {
  PHP_FE_END
};

void cassandra_define_UnavailableException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\UnavailableException", UnavailableException_methods);
  cassandra_unavailable_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_execution_exception_ce);
}
