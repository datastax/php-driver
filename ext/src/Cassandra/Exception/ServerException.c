#include "php_cassandra.h"

zend_class_entry *cassandra_server_exception_ce = NULL;

static zend_function_entry ServerException_methods[] = {
  PHP_FE_END
};

void cassandra_define_ServerException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ServerException", ServerException_methods);
  cassandra_server_exception_ce = zend_register_internal_class_ex(&ce, cassandra_runtime_exception_ce, NULL TSRMLS_CC);
}
