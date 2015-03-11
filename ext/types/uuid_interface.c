#include <php.h>
#include "../php_cassandra.h"
#include "uuid_interface.h"

zend_class_entry *cassandra_ce_UuidInterface = NULL;

static zend_function_entry CassandraUuidInterface_methods[] = {
  PHP_ABSTRACT_ME(CassandraUuidInterface, uuid, NULL)
  PHP_ABSTRACT_ME(CassandraUuidInterface, version, NULL)
  PHP_FE_END
};

void
cassandra_define_CassandraUuidInterface(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\UuidInterface", CassandraUuidInterface_methods);
  cassandra_ce_UuidInterface = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_UuidInterface->ce_flags |= ZEND_ACC_INTERFACE;
}
