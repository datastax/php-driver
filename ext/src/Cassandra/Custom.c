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

zend_class_entry* cassandra_custom_ce = NULL;

static zend_function_entry cassandra_custom_methods[] = {
  PHP_FE_END
};

void
cassandra_define_Custom(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Custom", cassandra_custom_methods);
  cassandra_custom_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_custom_ce TSRMLS_CC, 1, cassandra_value_ce);
  cassandra_custom_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
