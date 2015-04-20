#include "php_cassandra.h"

zend_class_entry *cassandra_authentication_exception_ce = NULL;

static zend_function_entry AuthenticationException_methods[] = {
  PHP_FE_END
};

void cassandra_define_AuthenticationException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\AuthenticationException", AuthenticationException_methods);
  cassandra_authentication_exception_ce = zend_register_internal_class_ex(&ce, cassandra_runtime_exception_ce, NULL TSRMLS_CC);
}
