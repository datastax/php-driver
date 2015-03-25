#include "../php_cassandra.h"
#include "exception.h"

extern zend_class_entry* cassandra_ce_RuntimeException;

zend_class_entry *cassandra_ce_TimeoutException = NULL;

static zend_function_entry CassandraTimeoutException_methods[] = {
  PHP_FE_END
};

void cassandra_define_CassandraTimeoutException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\TimeoutException", CassandraTimeoutException_methods);
  cassandra_ce_TimeoutException = zend_register_internal_class_ex(&ce, cassandra_ce_RuntimeException, NULL TSRMLS_CC);
}
