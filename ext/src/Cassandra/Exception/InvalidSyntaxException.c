#include "php_cassandra.h"

zend_class_entry *cassandra_invalid_syntax_exception_ce = NULL;

static zend_function_entry InvalidSyntaxException_methods[] = {
  PHP_FE_END
};

void cassandra_define_InvalidSyntaxException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\InvalidSyntaxException", InvalidSyntaxException_methods);
  cassandra_invalid_syntax_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_validation_exception_ce);
}
