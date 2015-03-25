#include "../php_cassandra.h"
#include "exception.h"

extern zend_class_entry* spl_ce_InvalidArgumentException;
extern zend_class_entry* cassandra_ce_Exception;

zend_class_entry *cassandra_ce_InvalidArgumentException = NULL;

static zend_function_entry CassandraInvalidArgumentException_methods[] = {
  PHP_FE_END
};

void cassandra_define_CassandraInvalidArgumentException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\InvalidArgumentException", CassandraInvalidArgumentException_methods);
  cassandra_ce_InvalidArgumentException = zend_register_internal_class_ex(&ce, spl_ce_InvalidArgumentException, NULL TSRMLS_CC);
  zend_class_implements(cassandra_ce_InvalidArgumentException TSRMLS_CC, 1, cassandra_ce_Exception);
}
