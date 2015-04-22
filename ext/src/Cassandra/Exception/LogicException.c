#include "../php_cassandra.h"
#include <ext/spl/spl_exceptions.h>

extern zend_class_entry* spl_ce_LogicException;

zend_class_entry *cassandra_logic_exception_ce = NULL;

static zend_function_entry LogicException_methods[] = {
  PHP_FE_END
};

void cassandra_define_LogicException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\LogicException", LogicException_methods);
  cassandra_logic_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_LogicException, NULL TSRMLS_CC);
  zend_class_implements(cassandra_logic_exception_ce TSRMLS_CC, 1, cassandra_exception_ce);
}
