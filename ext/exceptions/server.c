#include "../php_cassandra.h"
#include "exception.h"

extern zend_class_entry* cassandra_ce_RuntimeException;

zend_class_entry *cassandra_ce_ServerException = NULL;
zend_class_entry *cassandra_ce_OverloadedException = NULL;
zend_class_entry *cassandra_ce_IsBootstrappingException = NULL;

zend_class_entry *cassandra_ce_ExecutionException = NULL;
zend_class_entry *cassandra_ce_TruncateException = NULL;
zend_class_entry *cassandra_ce_WriteTimeoutException = NULL;
zend_class_entry *cassandra_ce_ReadTimeoutException = NULL;
zend_class_entry *cassandra_ce_UnavailableException = NULL;

zend_class_entry *cassandra_ce_ValidationException = NULL;
zend_class_entry *cassandra_ce_InvalidSyntaxException = NULL;
zend_class_entry *cassandra_ce_UnauthorizedException = NULL;
zend_class_entry *cassandra_ce_InvalidQueryException = NULL;
zend_class_entry *cassandra_ce_ConfigurationException = NULL;
zend_class_entry *cassandra_ce_AlreadyExistsException = NULL;
zend_class_entry *cassandra_ce_UnpreparedException = NULL;

zend_class_entry *cassandra_ce_ProtocolException = NULL;
zend_class_entry *cassandra_ce_AuthenticationException = NULL;

static zend_function_entry CassandraServerException_methods[] = {
  PHP_FE_END
};

void cassandra_define_CassandraServerException(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ServerException", CassandraServerException_methods);
  cassandra_ce_ServerException = zend_register_internal_class_ex(&ce, cassandra_ce_RuntimeException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\OverloadedException", CassandraServerException_methods);
  cassandra_ce_OverloadedException = zend_register_internal_class_ex(&ce, cassandra_ce_ServerException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\IsBootstrappingException", CassandraServerException_methods);
  cassandra_ce_IsBootstrappingException = zend_register_internal_class_ex(&ce, cassandra_ce_ServerException, NULL TSRMLS_CC);

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ExecutionException", CassandraServerException_methods);
  cassandra_ce_ExecutionException = zend_register_internal_class_ex(&ce, cassandra_ce_RuntimeException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\TruncateException", CassandraServerException_methods);
  cassandra_ce_TruncateException = zend_register_internal_class_ex(&ce, cassandra_ce_ExecutionException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\UnavailableException", CassandraServerException_methods);
  cassandra_ce_UnavailableException = zend_register_internal_class_ex(&ce, cassandra_ce_ExecutionException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ReadTimeoutException", CassandraServerException_methods);
  cassandra_ce_ReadTimeoutException = zend_register_internal_class_ex(&ce, cassandra_ce_ExecutionException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\WriteTimeoutException", CassandraServerException_methods);
  cassandra_ce_WriteTimeoutException = zend_register_internal_class_ex(&ce, cassandra_ce_ExecutionException, NULL TSRMLS_CC);

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ValidationException", CassandraServerException_methods);
  cassandra_ce_ValidationException = zend_register_internal_class_ex(&ce, cassandra_ce_RuntimeException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\InvalidSyntaxException", CassandraServerException_methods);
  cassandra_ce_InvalidSyntaxException = zend_register_internal_class_ex(&ce, cassandra_ce_ValidationException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\UnauthorizedException", CassandraServerException_methods);
  cassandra_ce_UnauthorizedException = zend_register_internal_class_ex(&ce, cassandra_ce_ValidationException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\InvalidQueryException", CassandraServerException_methods);
  cassandra_ce_InvalidQueryException = zend_register_internal_class_ex(&ce, cassandra_ce_ValidationException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ConfigurationException", CassandraServerException_methods);
  cassandra_ce_ConfigurationException = zend_register_internal_class_ex(&ce, cassandra_ce_ValidationException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\AlreadyExistsException", CassandraServerException_methods);
  cassandra_ce_AlreadyExistsException = zend_register_internal_class_ex(&ce, cassandra_ce_ConfigurationException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\UnpreparedException", CassandraServerException_methods);
  cassandra_ce_UnpreparedException = zend_register_internal_class_ex(&ce, cassandra_ce_ValidationException, NULL TSRMLS_CC);

  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\ProtocolException", CassandraServerException_methods);
  cassandra_ce_ProtocolException = zend_register_internal_class_ex(&ce, cassandra_ce_RuntimeException, NULL TSRMLS_CC);
  INIT_CLASS_ENTRY(ce, "Cassandra\\Exception\\AuthenticationException", CassandraServerException_methods);
  cassandra_ce_AuthenticationException = zend_register_internal_class_ex(&ce, cassandra_ce_RuntimeException, NULL TSRMLS_CC);
}
