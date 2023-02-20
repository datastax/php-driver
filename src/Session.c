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

zend_class_entry *php_driver_session_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, statement)
  ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_prepare, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, cql)
  ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_session_methods[] = {
  PHP_ABSTRACT_ME(Session, execute, arginfo_execute)
  PHP_ABSTRACT_ME(Session, executeAsync, arginfo_execute)
  PHP_ABSTRACT_ME(Session, prepare, arginfo_prepare)
  PHP_ABSTRACT_ME(Session, prepareAsync, arginfo_prepare)
  PHP_ABSTRACT_ME(Session, close, arginfo_timeout)
  PHP_ABSTRACT_ME(Session, closeAsync, arginfo_none)
  PHP_ABSTRACT_ME(Session, metrics, arginfo_none)
  PHP_ABSTRACT_ME(Session, schema, arginfo_none)
  PHP_FE_END
};

void php_driver_define_Session(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Session", php_driver_session_methods);
  php_driver_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  php_driver_session_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
