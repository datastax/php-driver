/**
 * Copyright 2015-2016 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "php_driver.h"
#include "php_driver_types.h"

zend_class_entry* php_driver_custom_ce = NULL;

static zend_function_entry php_driver_custom_methods[] = {
  PHP_FE_END
};

void
php_driver_define_Custom(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Custom", php_driver_custom_methods);
  php_driver_custom_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_custom_ce TSRMLS_CC, 1, php_driver_value_ce);
  php_driver_custom_ce->ce_flags |= ZEND_ACC_ABSTRACT;
}
