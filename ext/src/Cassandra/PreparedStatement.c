#include "php_cassandra.h"

zend_class_entry *cassandra_prepared_statement_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(PreparedStatement, __construct)
{
}

static zend_function_entry cassandra_prepared_statement_methods[] = {
  PHP_ME(PreparedStatement, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
  PHP_FE_END
};

static zend_object_handlers cassandra_prepared_statement_handlers;

static HashTable*
php_cassandra_prepared_statement_properties(zval *object TSRMLS_DC)
{
  cassandra_prepared_statement* statement = (cassandra_prepared_statement*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*                    props     = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_prepared_statement_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_prepared_statement_free(void *object TSRMLS_DC)
{
  cassandra_prepared_statement* statement = (cassandra_prepared_statement*) object;

  if (statement->prepared) {
    cass_prepared_free(statement->prepared);
  }

  zend_object_std_dtor(&statement->zval TSRMLS_CC);
  efree(statement);
}

static zend_object_value
php_cassandra_prepared_statement_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_prepared_statement *statement;

  statement = (cassandra_prepared_statement*) ecalloc(1, sizeof(cassandra_prepared_statement));

  zend_object_std_init(&statement->zval, class_type TSRMLS_CC);
  object_properties_init(&statement->zval, class_type);

  statement->type = CASSANDRA_PREPARED_STATEMENT;
  statement->prepared = NULL;

  retval.handle   = zend_objects_store_put(statement,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_prepared_statement_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_prepared_statement_handlers;

  return retval;
}

void cassandra_define_PreparedStatement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\PreparedStatement", cassandra_prepared_statement_methods);
  cassandra_prepared_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_prepared_statement_ce TSRMLS_CC, 1, cassandra_statement_ce);
  cassandra_prepared_statement_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_prepared_statement_ce->create_object = php_cassandra_prepared_statement_new;

  memcpy(&cassandra_prepared_statement_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_prepared_statement_handlers.get_properties  = php_cassandra_prepared_statement_properties;
  cassandra_prepared_statement_handlers.compare_objects = php_cassandra_prepared_statement_compare;
}
