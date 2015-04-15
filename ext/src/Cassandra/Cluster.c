#include "php_cassandra.h"

zend_class_entry *cassandra_cluster_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_keyspace, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_cluster_methods[] = {
  PHP_ABSTRACT_ME(Cluster, connect, arginfo_keyspace)
  PHP_ABSTRACT_ME(Cluster, connectAsync, arginfo_keyspace)
  PHP_FE_END
};

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(Cluster)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Cluster", cassandra_cluster_methods);
  cassandra_cluster_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_cluster_ce->ce_flags |= ZEND_ACC_INTERFACE;

  return SUCCESS;
}
