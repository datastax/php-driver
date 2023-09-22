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

zend_class_entry *php_driver_batch_statement_ce = NULL;

void php_driver_batch_statement_entry_dtor(php5to7_dtor dest)
{
#if PHP_MAJOR_VERSION >= 7
  php_driver_batch_statement_entry *batch_statement_entry = Z_PTR_P(dest);
#else
  php_driver_batch_statement_entry *batch_statement_entry = *((php_driver_batch_statement_entry **) dest);
#endif

  zval_ptr_dtor(&batch_statement_entry->statement);
  PHP5TO7_ZVAL_MAYBE_DESTROY(batch_statement_entry->arguments);

  efree(batch_statement_entry);
}

PHP_METHOD(BatchStatement, __construct)
{
  zval *type = NULL;
  php_driver_statement *self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &type) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_STATEMENT(getThis());

  if (type) {
    if (Z_TYPE_P(type) != IS_LONG) {
      INVALID_ARGUMENT(type, "one of " PHP_DRIVER_NAMESPACE "::BATCH_TYPE_*");
    }

    switch (Z_LVAL_P(type)) {
    case CASS_BATCH_TYPE_LOGGED:
    case CASS_BATCH_TYPE_UNLOGGED:
    case CASS_BATCH_TYPE_COUNTER:
      self->data.batch.type = (CassBatchType) Z_LVAL_P(type);
      break;
    default:
      INVALID_ARGUMENT(type, "one of " PHP_DRIVER_NAMESPACE "::BATCH_TYPE_*");
    }
  }
}

PHP_METHOD(BatchStatement, add)
{
  zval *statement = NULL;
  zval *arguments = NULL;
  php_driver_batch_statement_entry *batch_statement_entry = NULL;
  php_driver_statement *self = NULL;

#if PHP_MAJOR_VERSION >= 7
  zval entry;
#endif

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &arguments) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(statement) != IS_STRING &&
      (Z_TYPE_P(statement) != IS_OBJECT ||
       (!instanceof_function(Z_OBJCE_P(statement), php_driver_simple_statement_ce TSRMLS_CC) &&
        !instanceof_function(Z_OBJCE_P(statement), php_driver_prepared_statement_ce TSRMLS_CC)))) {
    INVALID_ARGUMENT(statement, "a string, an instance of "
                     PHP_DRIVER_NAMESPACE "\\SimpleStatement or an instance of "
                     PHP_DRIVER_NAMESPACE "\\PreparedStatement");
  }

  self = PHP_DRIVER_GET_STATEMENT(getThis());

  batch_statement_entry = (php_driver_batch_statement_entry *) ecalloc(1, sizeof(php_driver_batch_statement_entry));

  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(batch_statement_entry->statement), statement);

  if (arguments) {
    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(batch_statement_entry->arguments), arguments);
  }

#if PHP_MAJOR_VERSION >= 7
  ZVAL_PTR(&entry, batch_statement_entry);
  zend_hash_next_index_insert(&self->data.batch.statements, &entry);
#else
  zend_hash_next_index_insert(&self->data.batch.statements,
                              &batch_statement_entry, sizeof(php_driver_batch_statement_entry *),
                              NULL);
#endif
}

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_add, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, statement)
  ZEND_ARG_ARRAY_INFO(0, arguments, 1)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_batch_statement_methods[] = {
  PHP_ME(BatchStatement, __construct, arginfo__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(BatchStatement, add, arginfo_add, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_batch_statement_handlers;

static HashTable *
php_driver_batch_statement_properties(php7to8_object *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_batch_statement_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_batch_statement_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_statement *self = PHP5TO7_ZEND_OBJECT_GET(statement, object);

  zend_hash_destroy(&self->data.batch.statements);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_batch_statement_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_statement *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(statement, ce);

  self->type       = PHP_DRIVER_BATCH_STATEMENT;
  self->data.batch.type = CASS_BATCH_TYPE_LOGGED;
  zend_hash_init(&self->data.batch.statements, 0, NULL, (dtor_func_t) php_driver_batch_statement_entry_dtor, 0);

  PHP5TO7_ZEND_OBJECT_INIT_EX(statement, batch_statement, self, ce);
}

void php_driver_define_BatchStatement(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\BatchStatement", php_driver_batch_statement_methods);
  php_driver_batch_statement_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_batch_statement_ce TSRMLS_CC, 1, php_driver_statement_ce);
  php_driver_batch_statement_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_batch_statement_ce->create_object = php_driver_batch_statement_new;

  memcpy(&php_driver_batch_statement_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_batch_statement_handlers.get_properties  = php_driver_batch_statement_properties;
  PHP7TO8_COMPARE(php_driver_batch_statement_handlers, php_driver_batch_statement_compare);
  php_driver_batch_statement_handlers.clone_obj = NULL;
}
