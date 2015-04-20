#include "php_cassandra.h"

zend_class_entry *cassandra_is_bootstrapping_exception_ce = NULL;

static zend_function_entry IsBootstrappingException_methods[] = {
  PHP_FE_END
};

void cassandra_define_IsBootstrappingException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\IsBootstrappingException", IsBootstrappingException_methods);
  cassandra_is_bootstrapping_exception_ce = zend_register_internal_class_ex(&ce, cassandra_server_exception_ce, NULL TSRMLS_CC);
}
