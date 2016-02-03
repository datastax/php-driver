#include "php_cassandra.h"

zend_class_entry *cassandra_domain_exception_ce = NULL;

static zend_function_entry DomainException_methods[] = {
  PHP_FE_END
};

void cassandra_define_DomainException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\DomainException", DomainException_methods);
  cassandra_domain_exception_ce = php5to7_zend_register_internal_class_ex(&ce, spl_ce_DomainException);
  zend_class_implements(cassandra_domain_exception_ce TSRMLS_CC, 1, cassandra_exception_ce);
}
