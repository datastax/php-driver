#include "php_cassandra.h"

zend_class_entry *cassandra_unprepared_exception_ce = NULL;

static zend_function_entry UnpreparedException_methods[] = {
  PHP_FE_END
};

void cassandra_define_UnpreparedException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\UnpreparedException", UnpreparedException_methods);
  cassandra_unprepared_exception_ce = zend_register_internal_class_ex(&ce, cassandra_validation_exception_ce, NULL TSRMLS_CC);
}
