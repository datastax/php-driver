#include "php_cassandra.h"

zend_class_entry *cassandra_invalid_argument_exception_ce = NULL;

static zend_function_entry InvalidArgumentException_methods[] = {
  PHP_FE_END
};

void cassandra_define_InvalidArgumentException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\InvalidArgumentException", InvalidArgumentException_methods);
  cassandra_invalid_argument_exception_ce = php5to7_zend_register_internal_class_ex(&ce, spl_ce_InvalidArgumentException);
  zend_class_implements(cassandra_invalid_argument_exception_ce TSRMLS_CC, 1, cassandra_exception_ce);
}
