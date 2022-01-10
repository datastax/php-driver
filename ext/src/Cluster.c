/**
 * Copyright 2015-2017 DataStax, Inc.
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

zend_class_entry *php_driver_cluster_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_connect, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_connectAsync, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_cluster_methods[] = {
  PHP_ABSTRACT_ME(Cluster, connect, arginfo_connect)
  PHP_ABSTRACT_ME(Cluster, connectAsync, arginfo_connectAsync)
  PHP_FE_END
};

void php_driver_define_Cluster(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Cluster", php_driver_cluster_methods);
  php_driver_cluster_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_cluster_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
