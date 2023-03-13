#pragma once

#include <php.h>

BEGIN_EXTERN_C()
zend_object* php_driver_default_cluster_new(zend_class_entry *ce);
END_EXTERN_C()

void php_driver_initialize_default_cluster_handlers();