#include "php_cassandra.h"
#include <ext/spl/spl_exceptions.h>

extern zend_class_entry* spl_ce_RuntimeException;

zend_class_entry *cassandra_runtime_exception_ce = NULL;

static zend_function_entry RuntimeException_methods[] = {
  PHP_FE_END
};

void cassandra_define_RuntimeException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\RuntimeException", RuntimeException_methods);
  cassandra_runtime_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException, NULL TSRMLS_CC);
  zend_class_implements(cassandra_runtime_exception_ce TSRMLS_CC, 1, cassandra_exception_ce);
}
