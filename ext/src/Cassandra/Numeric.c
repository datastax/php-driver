#include "php_cassandra.h"

zend_class_entry* cassandra_numeric_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_num, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_numeric_methods[] = {
  PHP_ABSTRACT_ME(Numeric, add, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, sub, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, mul, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, div, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, mod, arginfo_num)
  PHP_ABSTRACT_ME(Numeric, abs, arginfo_none)
  PHP_ABSTRACT_ME(Numeric, neg, arginfo_none)
  PHP_ABSTRACT_ME(Numeric, sqrt, arginfo_none)
  PHP_ABSTRACT_ME(Numeric, toInt, arginfo_none)
  PHP_ABSTRACT_ME(Numeric, toDouble, arginfo_none)
  PHP_FE_END
};

void
cassandra_define_Numeric(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Numeric", cassandra_numeric_methods);
  cassandra_numeric_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_numeric_ce TSRMLS_CC, 1, cassandra_value_ce);
  cassandra_numeric_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
