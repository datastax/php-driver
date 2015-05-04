#include "php_cassandra.h"

zend_class_entry *cassandra_simple_statement_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(SimpleStatement, __construct)
{
  zval* cql = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &cql) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(cql) != IS_STRING) {
    INVALID_ARGUMENT(cql, "a string");
  }

  cassandra_simple_statement* self =
    (cassandra_simple_statement*) zend_object_store_get_object(getThis() TSRMLS_CC);

  self->cql = estrndup(Z_STRVAL_P(cql), Z_STRLEN_P(cql));
}

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, cql)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_simple_statement_methods[] = {
  PHP_ME(SimpleStatement, __construct, arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_FE_END
};

static zend_object_handlers cassandra_simple_statement_handlers;

static HashTable*
php_cassandra_simple_statement_properties(zval *object TSRMLS_DC)
{
  cassandra_simple_statement* statement = (cassandra_simple_statement*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*                  props     = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_simple_statement_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_simple_statement_free(void *object TSRMLS_DC)
{
  cassandra_simple_statement* statement = (cassandra_simple_statement*) object;

  if (statement->cql)
    efree(statement->cql);

  zend_object_std_dtor(&statement->zval TSRMLS_CC);
  efree(statement);
}

static zend_object_value
php_cassandra_simple_statement_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_simple_statement *statement;

  statement = (cassandra_simple_statement*) ecalloc(1, sizeof(cassandra_simple_statement));

  zend_object_std_init(&statement->zval, class_type TSRMLS_CC);
  object_properties_init(&statement->zval, class_type);

  statement->type = CASSANDRA_SIMPLE_STATEMENT;
  statement->cql  = NULL;

  retval.handle   = zend_objects_store_put(statement,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_simple_statement_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_simple_statement_handlers;

  return retval;
}

void cassandra_define_SimpleStatement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\SimpleStatement", cassandra_simple_statement_methods);
  cassandra_simple_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_simple_statement_ce TSRMLS_CC, 1, cassandra_statement_ce);
  cassandra_simple_statement_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_simple_statement_ce->create_object = php_cassandra_simple_statement_new;

  memcpy(&cassandra_simple_statement_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_simple_statement_handlers.get_properties  = php_cassandra_simple_statement_properties;
  cassandra_simple_statement_handlers.compare_objects = php_cassandra_simple_statement_compare;
}
