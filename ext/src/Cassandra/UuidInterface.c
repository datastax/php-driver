#include "php_cassandra.h"

zend_class_entry *cassandra_uuid_interface_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_uuid_interface_methods[] = {
  PHP_ABSTRACT_ME(UuidInterface, uuid, arginfo_none)
  PHP_ABSTRACT_ME(UuidInterface, version, arginfo_none)
  PHP_FE_END
};

void
cassandra_define_UuidInterface(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\UuidInterface", cassandra_uuid_interface_methods);
  cassandra_uuid_interface_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_uuid_interface_ce TSRMLS_CC, 1, cassandra_value_ce);
  cassandra_uuid_interface_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
