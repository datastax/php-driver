#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cassandra.h"

ZEND_DECLARE_MODULE_GLOBALS(cassandra)

const zend_function_entry cassandra_functions[] = {
  PHP_FE(example, NULL)
  PHP_FE_END /* Must be the last line in cassandra_functions[] */
};

zend_module_entry cassandra_module_entry = {
  STANDARD_MODULE_HEADER,
  "Cassandra",
  cassandra_functions,
  PHP_MINIT(cassandra),
  PHP_MSHUTDOWN(cassandra),
  PHP_RINIT(cassandra),
  PHP_RSHUTDOWN(cassandra),
  PHP_MINFO(cassandra),
  "0.1",
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CASSANDRA
ZEND_GET_MODULE(cassandra)
#endif

PHP_MINIT_FUNCTION(cassandra)
{
  REGISTER_INI_ENTRIES();
  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cassandra)
{
  UNREGISTER_INI_ENTRIES();
  return SUCCESS;
}

PHP_RINIT_FUNCTION(cassandra)
{
  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(cassandra)
{
  return SUCCESS;
}


PHP_MINFO_FUNCTION(cassandra)
{
  php_info_print_table_start( );
  php_info_print_table_header(2, "Cassandra support", "enabled");
  php_info_print_table_end( );
}

PHP_FUNCTION(example)
{
  long number;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &number) != SUCCESS) {
    return;
  }

  number *= 2;

  RETURN_LONG(number);
}
