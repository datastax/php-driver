#include "php_cassandra.h"

zend_class_entry *cassandra_timestamp_gen_ce = NULL;

static zend_function_entry cassandra_timestamp_gen_methods[] = {
  PHP_FE_END
};

void
cassandra_define_TimestampGenerator(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\TimestampGenerator", cassandra_timestamp_gen_methods);
  cassandra_timestamp_gen_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_timestamp_gen_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
