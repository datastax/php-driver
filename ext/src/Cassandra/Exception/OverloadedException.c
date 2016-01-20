#include "php_cassandra.h"

zend_class_entry *cassandra_overloaded_exception_ce = NULL;

static zend_function_entry OverloadedException_methods[] = {
  PHP_FE_END
};

void cassandra_define_OverloadedException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\OverloadedException", OverloadedException_methods);
  cassandra_overloaded_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_server_exception_ce);
}
