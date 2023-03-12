#pragma once

#include <php.h>
#include <php_driver_types.h>

BEGIN_EXTERN_C()
int php_driver_execution_options_build_local_from_array(php_driver_execution_options *self, zval *options);
END_EXTERN_C()
