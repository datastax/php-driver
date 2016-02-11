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

#include "php_cassandra.h"

zend_class_entry *cassandra_table_ce = NULL;

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_table_methods[] = {
  PHP_ABSTRACT_ME(Table, name, arginfo_none)
  PHP_ABSTRACT_ME(Table, comment, arginfo_none)
  PHP_ABSTRACT_ME(Table, readRepairChance, arginfo_none)
  PHP_ABSTRACT_ME(Table, localReadRepairChance, arginfo_none)
  PHP_ABSTRACT_ME(Table, gcGraceSeconds, arginfo_none)
  PHP_ABSTRACT_ME(Table, caching, arginfo_none)
  PHP_ABSTRACT_ME(Table, bloomFilterFPChance, arginfo_none)
  PHP_ABSTRACT_ME(Table, memtableFlushPeriodMs, arginfo_none)
  PHP_ABSTRACT_ME(Table, defaultTTL, arginfo_none)
  PHP_ABSTRACT_ME(Table, speculativeRetry, arginfo_none)
  PHP_ABSTRACT_ME(Table, indexInterval, arginfo_none)
  PHP_ABSTRACT_ME(Table, compactionStrategyClassName, arginfo_none)
  PHP_ABSTRACT_ME(Table, compactionStrategyOptions, arginfo_none)
  PHP_ABSTRACT_ME(Table, compressionParameters, arginfo_none)
  PHP_ABSTRACT_ME(Table, populateIOCacheOnFlush, arginfo_none)
  PHP_ABSTRACT_ME(Table, replicateOnWrite, arginfo_none)
  PHP_ABSTRACT_ME(Table, maxIndexInterval, arginfo_none)
  PHP_ABSTRACT_ME(Table, minIndexInterval, arginfo_none)
  PHP_ABSTRACT_ME(Table, column, arginfo_name)
  PHP_ABSTRACT_ME(Table, columns, arginfo_none)
  PHP_FE_END
};

void cassandra_define_Table(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Table", cassandra_table_methods);
  cassandra_table_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_table_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
