#include "php_cassandra.h"

zend_class_entry *cassandra_already_exists_exception_ce = NULL;

static zend_function_entry AlreadyExistsException_methods[] = {
  PHP_FE_END
};

void cassandra_define_AlreadyExistsException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\AlreadyExistsException", AlreadyExistsException_methods);
  cassandra_already_exists_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_configuration_exception_ce);
}
