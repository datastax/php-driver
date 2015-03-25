#include "../php_cassandra.h"
#include "exception.h"

extern zend_class_entry* spl_ce_LogicException;
extern zend_class_entry* cassandra_ce_Exception;

zend_class_entry *cassandra_ce_LogicException = NULL;

static zend_function_entry CassandraLogicException_methods[] = {
  PHP_FE_END
};

void cassandra_define_CassandraLogicException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\LogicException", CassandraLogicException_methods);
  cassandra_ce_LogicException = zend_register_internal_class_ex(&ce, spl_ce_LogicException, NULL TSRMLS_CC);
  zend_class_implements(cassandra_ce_LogicException TSRMLS_CC, 1, cassandra_ce_Exception);
}
