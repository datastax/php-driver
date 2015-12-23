#include "php_cassandra.h"

zend_class_entry *cassandra_unauthorized_exception_ce = NULL;

static zend_function_entry UnauthorizedException_methods[] = {
  PHP_FE_END
};

void cassandra_define_UnauthorizedException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\UnauthorizedException", UnauthorizedException_methods);
  cassandra_unauthorized_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_validation_exception_ce);
}
