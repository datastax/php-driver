#ifndef PHP_DRIVER_EXECUTION_OPTIONS_H
#define PHP_DRIVER_EXECUTION_OPTIONS_H

#include <php.h>
#include <php_driver_types.h>

int php_driver_execution_options_build_local_from_array(php_driver_execution_options *self, zval *options);

#endif
