#include "../php_cassandra.h"
#include "exception.h"

extern zend_class_entry* spl_ce_RuntimeException;
extern zend_class_entry* cassandra_ce_Exception;

zend_class_entry *cassandra_ce_RuntimeException = NULL;

static zend_function_entry CassandraRuntimeException_methods[] = {
  PHP_FE_END
};

void cassandra_define_CassandraRuntimeException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\RuntimeException", CassandraRuntimeException_methods);
  cassandra_ce_RuntimeException = zend_register_internal_class_ex(&ce, spl_ce_RuntimeException, NULL TSRMLS_CC);
  zend_class_implements(cassandra_ce_RuntimeException TSRMLS_CC, 1, cassandra_ce_Exception);
}
