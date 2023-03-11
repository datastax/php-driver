#pragma once

#include <php.h>

BEGIN_EXTERN_C()
void php_driver_initialize_cluster_builder_handlers();
zend_object *php_driver_cluster_builder_new(zend_class_entry *ce);
END_EXTERN_C()