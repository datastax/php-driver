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
#include "php_driver_types.h"

zend_class_entry* php_driver_core_ce = NULL;

PHP_DRIVER_CORE_METHOD(cluster)
{
  object_init_ex(return_value, php_driver_cluster_builder_ce);
}

PHP_DRIVER_CORE_METHOD(ssl)
{
  object_init_ex(return_value, php_driver_ssl_builder_ce);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry core_methods[] = {
  PHP_DRIVER_CORE_ME(cluster, arginfo_none, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_DRIVER_CORE_ME(ssl, arginfo_none, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
      PHP_FE_END
};

void
php_driver_define_Core(TSRMLS_D)
{
  zend_class_entry ce;
  char buf[64];

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE, core_methods);
  php_driver_core_ce = zend_register_internal_class(&ce);
  php_driver_core_ce->ce_flags |= ZEND_ACC_FINAL;

  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_ANY"), CASS_CONSISTENCY_ANY);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_ONE"), CASS_CONSISTENCY_ONE);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_TWO"), CASS_CONSISTENCY_TWO);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_THREE"), CASS_CONSISTENCY_THREE);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_QUORUM"), CASS_CONSISTENCY_QUORUM);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_ALL"), CASS_CONSISTENCY_ALL);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_LOCAL_QUORUM"), CASS_CONSISTENCY_LOCAL_QUORUM);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_EACH_QUORUM"), CASS_CONSISTENCY_EACH_QUORUM);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_SERIAL"), CASS_CONSISTENCY_SERIAL);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_LOCAL_SERIAL"), CASS_CONSISTENCY_LOCAL_SERIAL);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("CONSISTENCY_LOCAL_ONE"), CASS_CONSISTENCY_LOCAL_ONE);

  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("VERIFY_NONE"), CASS_SSL_VERIFY_NONE);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("VERIFY_PEER_CERT"), CASS_SSL_VERIFY_PEER_CERT);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("VERIFY_PEER_IDENTITY"), CASS_SSL_VERIFY_PEER_IDENTITY);

  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("BATCH_LOGGED"), CASS_BATCH_TYPE_LOGGED);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("BATCH_UNLOGGED"), CASS_BATCH_TYPE_UNLOGGED);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("BATCH_COUNTER"), CASS_BATCH_TYPE_COUNTER);

  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("LOG_DISABLED"), CASS_LOG_DISABLED);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("LOG_CRITICAL"), CASS_LOG_CRITICAL);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("LOG_ERROR"), CASS_LOG_ERROR);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("LOG_WARN"), CASS_LOG_WARN);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("LOG_INFO"), CASS_LOG_INFO);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("LOG_DEBUG"), CASS_LOG_DEBUG);
  zend_declare_class_constant_long(php_driver_core_ce, ZEND_STRL("LOG_TRACE"), CASS_LOG_TRACE);

  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_TEXT"), "text");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_ASCII"), "ascii");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_VARCHAR"), "varchar");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_BIGINT"), "bigint");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_SMALLINT"), "smallint");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_TINYINT"), "tinyint");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_BLOB"), "blob");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_BOOLEAN"), "boolean");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_COUNTER"), "counter");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_DECIMAL"), "decimal");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_DOUBLE"), "double");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_FLOAT"), "float");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_INT"), "int");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_TIMESTAMP"), "timestamp");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_UUID"), "uuid");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_VARINT"), "varint");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_TIMEUUID"), "timeuuid");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("TYPE_INET"), "inet");

  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("VERSION"), PHP_DRIVER_VERSION_FULL);

  snprintf(buf, sizeof(buf), "%d.%d.%d%s",
           CASS_VERSION_MAJOR, CASS_VERSION_MINOR, CASS_VERSION_PATCH,
           strlen(CASS_VERSION_SUFFIX) > 0 ? "-" CASS_VERSION_SUFFIX : "");
  zend_declare_class_constant_string(php_driver_core_ce, ZEND_STRL("CPP_DRIVER_VERSION"), buf);
}
