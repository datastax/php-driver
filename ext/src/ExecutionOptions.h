#ifndef PHP_DRIVER_EXECUTION_OPTIONS_H
#define PHP_DRIVER_EXECUTION_OPTIONS_H

int php_driver_execution_options_init_and_build_from_array(php_driver_execution_options *self, zval *options TSRMLS_DC);

void php_driver_execution_options_destroy(php_driver_execution_options *self);

#endif
