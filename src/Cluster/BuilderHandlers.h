#pragma once

#include <php.h>

void php_driver_initialize_cluster_builder_handlers();

BEGIN_EXTERN_C()
zend_object *php_driver_cluster_builder_new(zend_class_entry *ce);
END_EXTERN_C()