#include "php_cassandra.h"
#include "util/types.h"

zend_class_entry *cassandra_retry_policy_downgrading_consistency_ce = NULL;

static zend_function_entry cassandra_retry_policy_downgrading_consistency_methods[] = {
  PHP_FE_END
};

static zend_object_handlers cassandra_retry_policy_downgrading_consistency_handlers;

static void
php_cassandra_retry_policy_downgrading_consistency_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_retry_policy *self = PHP5TO7_ZEND_OBJECT_GET(retry_policy, object);

  cass_retry_policy_free(self->policy);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_retry_policy_downgrading_consistency_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_retry_policy *self = PHP5TO7_ZEND_OBJECT_ECALLOC(retry_policy, ce);

  self->policy = cass_retry_policy_downgrading_consistency_new();

  PHP5TO7_ZEND_OBJECT_INIT_EX(retry_policy, retry_policy_downgrading_consistency, self, ce);
}

void cassandra_define_RetryPolicyDowngradingConsistency(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\RetryPolicy\\DowngradingConsistency", cassandra_retry_policy_downgrading_consistency_methods);
  cassandra_retry_policy_downgrading_consistency_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_retry_policy_downgrading_consistency_ce TSRMLS_CC, 1, cassandra_retry_policy_ce);
  cassandra_retry_policy_downgrading_consistency_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_retry_policy_downgrading_consistency_ce->create_object = php_cassandra_retry_policy_downgrading_consistency_new;

  memcpy(&cassandra_retry_policy_downgrading_consistency_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
