#include "php_cassandra.h"

zend_class_entry *cassandra_configuration_exception_ce = NULL;

static zend_function_entry ConfigurationException_methods[] = {
  PHP_FE_END
};

void cassandra_define_ConfigurationException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ConfigurationException", ConfigurationException_methods);
  cassandra_configuration_exception_ce = zend_register_internal_class_ex(&ce, cassandra_validation_exception_ce, NULL TSRMLS_CC);
}
