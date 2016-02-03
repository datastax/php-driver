#include "php_cassandra.h"
#include <ext/spl/spl_exceptions.h>

zend_class_entry *cassandra_range_exception_ce = NULL;

static zend_function_entry RangeException_methods[] = {
  PHP_FE_END
};

void cassandra_define_RangeException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\RangeException", RangeException_methods);
  cassandra_range_exception_ce = php5to7_zend_register_internal_class_ex(&ce, spl_ce_RangeException);
  zend_class_implements(cassandra_range_exception_ce TSRMLS_CC, 1, cassandra_exception_ce);
}
