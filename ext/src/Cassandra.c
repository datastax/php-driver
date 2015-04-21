#include "php_cassandra.h"

zend_class_entry *cassandra_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(Cassandra, cluster)
{
  object_init_ex(return_value, cassandra_cluster_builder_ce);
}

PHP_METHOD(Cassandra, ssl)
{
  object_init_ex(return_value, cassandra_ssl_builder_ce);
}

PHP_METHOD(Cassandra, setLogLevel)
{
  zval* level;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &level) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(level) != IS_LONG) {
    INVALID_ARGUMENT(level, "one of Cassandra::LOG_*");
  }

  switch (Z_LVAL_P(level)) {
  case CASS_LOG_DISABLED:
  case CASS_LOG_CRITICAL:
  case CASS_LOG_ERROR:
  case CASS_LOG_WARN:
  case CASS_LOG_INFO:
  case CASS_LOG_DEBUG:
  case CASS_LOG_TRACE:
    break;
  default:
    INVALID_ARGUMENT(level, "one of Cassandra::LOG_*");
  }

  CASSANDRA_G(log_level) = Z_LVAL_P(level);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_level, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, level)
ZEND_END_ARG_INFO()

static zend_function_entry Cassandra_methods[] = {
  PHP_ME(Cassandra, cluster,     arginfo_none,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_ME(Cassandra, ssl,         arginfo_none,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_ME(Cassandra, setLogLevel, arginfo_level, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
  PHP_FE_END
};

void cassandra_define_Cassandra(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra", Cassandra_methods);
  cassandra_ce = zend_register_internal_class(&ce TSRMLS_CC);

  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_ANY"),          CASS_CONSISTENCY_ANY TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_ONE"),          CASS_CONSISTENCY_ONE TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_TWO"),          CASS_CONSISTENCY_TWO TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_THREE"),        CASS_CONSISTENCY_THREE TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_QUORUM"),       CASS_CONSISTENCY_QUORUM TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_ALL"),          CASS_CONSISTENCY_ALL TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_LOCAL_QUORUM"), CASS_CONSISTENCY_LOCAL_QUORUM TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_EACH_QUORUM"),  CASS_CONSISTENCY_EACH_QUORUM TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_SERIAL"),       CASS_CONSISTENCY_SERIAL TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_LOCAL_SERIAL"), CASS_CONSISTENCY_LOCAL_SERIAL TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("CONSISTENCY_LOCAL_ONE"),    CASS_CONSISTENCY_LOCAL_ONE TSRMLS_CC);

  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("VERIFY_NONE"),          CASS_SSL_VERIFY_NONE TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("VERIFY_PEER_CERT"),     CASS_SSL_VERIFY_PEER_CERT TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("VERIFY_PEER_IDENTITY"), CASS_SSL_VERIFY_PEER_IDENTITY TSRMLS_CC);

  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("BATCH_LOGGED"),   CASS_BATCH_TYPE_LOGGED TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("BATCH_UNLOGGED"), CASS_BATCH_TYPE_UNLOGGED TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("BATCH_COUNTER"),  CASS_BATCH_TYPE_COUNTER TSRMLS_CC);

  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOG_DISABLED"), CASS_LOG_DISABLED TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOG_CRITICAL"), CASS_LOG_CRITICAL TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOG_ERROR"),    CASS_LOG_ERROR TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOG_WARN"),     CASS_LOG_WARN TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOG_INFO"),     CASS_LOG_INFO TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOG_DEBUG"),    CASS_LOG_DEBUG TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOG_TRACE"),    CASS_LOG_TRACE TSRMLS_CC);

  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_TEXT"),      "text" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_ASCII"),     "ascii" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_VARCHAR"),   "varchar" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_BIGINT"),    "bigint" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_BLOB"),      "blob" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_BOOLEAN"),   "boolean" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_COUNTER"),   "counter" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_DECIMAL"),   "decimal" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_DOUBLE"),    "double" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_FLOAT"),     "float" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_INT"),       "int" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_TIMESTAMP"), "timestamp" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_UUID"),      "uuid" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_VARINT"),    "varint" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_TIMEUUID"),  "timeuuid" TSRMLS_CC);
  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("TYPE_INET"),      "inet" TSRMLS_CC);

  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOAD_BALANCING_ROUND_ROBIN"),          0 TSRMLS_CC);
  zend_declare_class_constant_long(cassandra_ce, ZEND_STRL("LOAD_BALANCING_DC_AWARE_ROUND_ROBIN"), 1 TSRMLS_CC);

  zend_declare_class_constant_string(cassandra_ce, ZEND_STRL("VERSION"), "1.0.0-beta" TSRMLS_CC);
}
