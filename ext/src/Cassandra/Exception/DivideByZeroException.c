#include "php_cassandra.h"

zend_class_entry *cassandra_divide_by_zero_exception_ce = NULL;

static zend_function_entry DivideByZeroException_methods[] = {
  PHP_FE_END
};

void cassandra_define_DivideByZeroException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\DivideByZeroException", DivideByZeroException_methods);
  cassandra_divide_by_zero_exception_ce = php5to7_zend_register_internal_class_ex(&ce, cassandra_range_exception_ce);
  zend_class_implements(cassandra_divide_by_zero_exception_ce TSRMLS_CC, 1, cassandra_exception_ce);
}
