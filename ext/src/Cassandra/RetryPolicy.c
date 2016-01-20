#include "php_cassandra.h"
#include "util/types.h"

zend_class_entry *cassandra_retry_policy_ce = NULL;

static zend_function_entry cassandra_retry_policy_methods[] = {
  PHP_FE_END
};

void cassandra_define_RetryPolicy(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\RetryPolicy", cassandra_retry_policy_methods);
  cassandra_retry_policy_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_retry_policy_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
