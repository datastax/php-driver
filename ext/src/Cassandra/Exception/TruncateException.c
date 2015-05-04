#include "php_cassandra.h"

zend_class_entry *cassandra_truncate_exception_ce = NULL;

static zend_function_entry TruncateException_methods[] = {
  PHP_FE_END
};

void cassandra_define_TruncateException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\TruncateException", TruncateException_methods);
  cassandra_truncate_exception_ce = zend_register_internal_class_ex(&ce, cassandra_execution_exception_ce, NULL TSRMLS_CC);
}
