#include "php_cassandra.h"

zend_class_entry *cassandra_keyspace_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_keyspace_methods[] = {
  PHP_ABSTRACT_ME(Keyspace, name, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, replicationClassName, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, replicationOptions, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, hasDurableWrites, arginfo_none)
  PHP_ABSTRACT_ME(Keyspace, table, arginfo_name)
  PHP_ABSTRACT_ME(Keyspace, tables, arginfo_none)
  PHP_FE_END
};

void cassandra_define_Keyspace(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Keyspace", cassandra_keyspace_methods);
  cassandra_keyspace_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_keyspace_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
