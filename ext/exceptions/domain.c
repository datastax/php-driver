#include "../php_cassandra.h"
#include "exception.h"

extern zend_class_entry* spl_ce_DomainException;
extern zend_class_entry* cassandra_ce_Exception;

zend_class_entry *cassandra_ce_DomainException = NULL;

static zend_function_entry CassandraDomainException_methods[] = {
  PHP_FE_END
};

void cassandra_define_CassandraDomainException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\DomainException", CassandraDomainException_methods);
  cassandra_ce_DomainException = zend_register_internal_class_ex(&ce, spl_ce_DomainException, NULL TSRMLS_CC);
  zend_class_implements(cassandra_ce_DomainException TSRMLS_CC, 1, cassandra_ce_Exception);
}
