#include "php_cassandra.h"

zend_class_entry *cassandra_batch_statement_ce = NULL;

void cassandra_batch_statement_entry_dtor(void** dest)
{
  cassandra_batch_statement_entry* entry = *((cassandra_batch_statement_entry**)dest);

  zval_ptr_dtor(&entry->statement);
  if (entry->arguments) {
    zval_ptr_dtor(&entry->arguments);
  }

  efree(entry);
}

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(BatchStatement, __construct)
{
  zval* type = NULL;
  cassandra_batch_statement* self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &type) == FAILURE) {
    return;
  }

  self = (cassandra_batch_statement*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (type) {
    if (Z_TYPE_P(type) != IS_LONG) {
      INVALID_ARGUMENT(type, "one of Cassandra::BATCH_TYPE_*");
      return;
    }

    switch (Z_LVAL_P(type)) {
    case CASS_BATCH_TYPE_LOGGED:
    case CASS_BATCH_TYPE_UNLOGGED:
    case CASS_BATCH_TYPE_COUNTER:
      self->batch_type = (CassBatchType) Z_LVAL_P(type);
      break;
    default:
      INVALID_ARGUMENT(type, "one of Cassandra::BATCH_TYPE_*");
      return;
    }
  }
}

PHP_METHOD(BatchStatement, add)
{
  zval* statement = NULL;
  zval* arguments = NULL;
  cassandra_batch_statement_entry* entry = NULL;
  cassandra_batch_statement* self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &arguments) == FAILURE) {
    return;
  }

  if (!instanceof_function(Z_OBJCE_P(statement), cassandra_simple_statement_ce TSRMLS_CC) &&
      !instanceof_function(Z_OBJCE_P(statement), cassandra_prepared_statement_ce TSRMLS_CC)) {
    INVALID_ARGUMENT(statement, "a Cassandra\\SimpleStatement or Cassandra\\PreparedStatement");
  }

  entry = (cassandra_batch_statement_entry*) ecalloc(1, sizeof(cassandra_batch_statement_entry));
  entry->statement = statement;
  Z_ADDREF_P(entry->statement);

  if (arguments) {
    entry->arguments = arguments;
    Z_ADDREF_P(entry->arguments);
  }

  self = (cassandra_batch_statement*) zend_object_store_get_object(getThis() TSRMLS_CC);

  zend_hash_next_index_insert(self->statements, &entry, sizeof(cassandra_batch_statement_entry*), NULL);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, statement, Cassandra\\Statement, 0)
  ZEND_ARG_ARRAY_INFO(0, arguments, 1)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_batch_statement_methods[] = {
  PHP_ME(BatchStatement, __construct, arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(BatchStatement, add, arginfo_add, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_batch_statement_handlers;

static HashTable*
php_cassandra_batch_statement_properties(zval *object TSRMLS_DC)
{
  cassandra_batch_statement* self = (cassandra_batch_statement*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*                 props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_batch_statement_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_batch_statement_free(void *object TSRMLS_DC)
{
  cassandra_batch_statement* self = (cassandra_batch_statement*) object;

  zend_hash_destroy(self->statements);
  FREE_HASHTABLE(self->statements);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  efree(self);
}

static zend_object_value
php_cassandra_batch_statement_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_batch_statement* self;

  self = (cassandra_batch_statement*) ecalloc(1, sizeof(cassandra_batch_statement));

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  self->type = CASSANDRA_BATCH_STATEMENT;
  self->batch_type = CASS_BATCH_TYPE_LOGGED;

  ALLOC_HASHTABLE(self->statements);
  zend_hash_init(self->statements, 0, NULL, (dtor_func_t) cassandra_batch_statement_entry_dtor, 0);

  retval.handle   = zend_objects_store_put(self,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_batch_statement_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_batch_statement_handlers;

  return retval;
}

void cassandra_define_BatchStatement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\BatchStatement", cassandra_batch_statement_methods);
  cassandra_batch_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_batch_statement_ce TSRMLS_CC, 1, cassandra_statement_ce);
  cassandra_batch_statement_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_batch_statement_ce->create_object = php_cassandra_batch_statement_new;

  memcpy(&cassandra_batch_statement_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_batch_statement_handlers.get_properties  = php_cassandra_batch_statement_properties;
  cassandra_batch_statement_handlers.compare_objects = php_cassandra_batch_statement_compare;
}
