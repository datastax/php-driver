#include "php_cassandra.h"
#include <ext/spl/spl_exceptions.h>

extern zend_class_entry* spl_ce_InvalidArgumentException;
extern zend_class_entry* cassandra_exception_ce;

zend_class_entry *cassandra_invalid_argument_exception_ce = NULL;

static zend_function_entry InvalidArgumentException_methods[] = {
  PHP_FE_END
};

void cassandra_define_InvalidArgumentException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\InvalidArgumentException", InvalidArgumentException_methods);
  cassandra_invalid_argument_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_InvalidArgumentException, NULL TSRMLS_CC);
  zend_class_implements(cassandra_invalid_argument_exception_ce TSRMLS_CC, 1, cassandra_exception_ce);
}
