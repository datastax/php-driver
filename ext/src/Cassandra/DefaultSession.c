#include "php_cassandra.h"
#include "util/bytes.h"
#include "util/future.h"
#include "util/result.h"
#include "util/ref.h"
#include "util/math.h"
#include "util/collections.h"

zend_class_entry *cassandra_default_session_ce = NULL;

#define CHECK_RESULT(rc) \
{ \
  if (rc != CASS_OK) { \
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC, \
                            "%s", cass_error_desc(rc)); \
    return FAILURE; \
  } \
  return SUCCESS; \
}

static int
bind_argument_by_index(CassStatement* statement, size_t index, zval* value TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_NULL)
    CHECK_RESULT(cass_statement_bind_null(statement, index));

  if (Z_TYPE_P(value) == IS_STRING)
    CHECK_RESULT(cass_statement_bind_string(statement, index, Z_STRVAL_P(value)));

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(cass_statement_bind_double(statement, index, Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(cass_statement_bind_int32(statement, index, Z_LVAL_P(value)));

  if (Z_TYPE_P(value) == IS_BOOL)
    CHECK_RESULT(cass_statement_bind_bool(statement, index, Z_BVAL_P(value)));

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), cassandra_float_ce TSRMLS_CC)) {
      cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_float(statement, index, float_number->value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_bigint_ce TSRMLS_CC)) {
      cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_int64(statement, index, bigint->value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_timestamp_ce TSRMLS_CC)) {
      cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_int64(statement, index, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_blob_ce TSRMLS_CC)) {
      cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_bytes(statement, index, blob->data, blob->size));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_varint_ce TSRMLS_CC)) {
      cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
      size_t size;
      cass_byte_t* data = export_twos_complement(varint->value, &size);
      CassError rc = cass_statement_bind_bytes(statement, index, data, size);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_decimal_ce TSRMLS_CC)) {
      cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
      size_t size;
      cass_byte_t* data;
      data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
      CassError rc = cass_statement_bind_decimal(statement, index, data, size, decimal->scale);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_uuid_interface_ce TSRMLS_CC)) {
      cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_uuid(statement, index, uuid->uuid));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_inet_ce TSRMLS_CC)) {
      cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_inet(statement, index, inet->inet));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_set_ce TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_set* set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_set(set, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_map_ce TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_map* map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_map(map, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_collection_ce TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_collection* coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_collection(coll, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int
bind_argument_by_name(CassStatement* statement, const char* name, zval* value TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_NULL) {
    CHECK_RESULT(cass_statement_bind_null_by_name(statement, name));
  }

  if (Z_TYPE_P(value) == IS_STRING)
    CHECK_RESULT(cass_statement_bind_string_by_name(statement, name, Z_STRVAL_P(value)));

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(cass_statement_bind_double_by_name(statement, name, Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(cass_statement_bind_int32_by_name(statement, name, Z_LVAL_P(value)));

  if (Z_TYPE_P(value) == IS_BOOL)
    CHECK_RESULT(cass_statement_bind_bool_by_name(statement, name, Z_BVAL_P(value)));

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), cassandra_float_ce TSRMLS_CC)) {
      cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_float_by_name(statement, name, float_number->value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_bigint_ce TSRMLS_CC)) {
      cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, name, bigint->value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_timestamp_ce TSRMLS_CC)) {
      cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, name, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_blob_ce TSRMLS_CC)) {
      cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_bytes_by_name(statement, name, blob->data, blob->size));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_varint_ce TSRMLS_CC)) {
      cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
      size_t size;
      cass_byte_t* data;
      data = (cass_byte_t*) export_twos_complement(varint->value, &size);
      CassError rc = cass_statement_bind_bytes_by_name(statement, name, data, size);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_decimal_ce TSRMLS_CC)) {
      cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
      size_t size;
      cass_byte_t* data;
      data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
      CassError rc = cass_statement_bind_decimal_by_name(statement, name, data, size, decimal->scale);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_uuid_interface_ce TSRMLS_CC)) {
      cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_uuid_by_name(statement, name, uuid->uuid));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_inet_ce TSRMLS_CC)) {
      cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_inet_by_name(statement, name, inet->inet));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_set_ce TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_set* set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_set(set, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_map_ce TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_map* map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_map(map, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_collection_ce TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_collection* coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_collection(coll, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int
bind_arguments(CassStatement* statement, HashTable* arguments TSRMLS_DC)
{
  HashPointer ptr;
  ulong       hashIndex   = 0;
  char*       hashKey     = NULL;
  int         hashKeyType = 0;
  zval**      value;
  int         rc          = SUCCESS;

  zend_hash_get_pointer(arguments, &ptr);
  zend_hash_internal_pointer_reset(arguments);

  while (zend_hash_get_current_data(arguments, (void**) &value) == SUCCESS) {
    switch (zend_hash_get_current_key(arguments, &hashKey, &hashIndex, 0)) {
    case HASH_KEY_IS_STRING:
      rc = bind_argument_by_name(statement, hashKey, *value TSRMLS_CC);
      break;
    case HASH_KEY_IS_LONG:
      rc = bind_argument_by_index(statement, hashIndex, *value TSRMLS_CC);
      break;
    default:
      zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC,
        "Unable to find name or index of the argument.");
      return FAILURE;
    }

    if (rc == FAILURE)
      break;

    zend_hash_move_forward(arguments);
  }

  zend_hash_set_pointer(arguments, &ptr);

  return rc;
}

static CassStatement*
create_statement(cassandra_statement* statement, HashTable* arguments TSRMLS_DC)
{
  CassStatement* stmt;
  zend_uint count;
  cassandra_simple_statement* simple;
  cassandra_prepared_statement* prepared;

  switch (statement->type) {
  case CASSANDRA_SIMPLE_STATEMENT:
    simple = (cassandra_simple_statement*) statement;
    count  = 0;

    if (arguments)
      count = zend_hash_num_elements(arguments);

    stmt = cass_statement_new(simple->cql, count);
    break;
  case CASSANDRA_PREPARED_STATEMENT:
    prepared = (cassandra_prepared_statement*) statement;
    stmt = cass_prepared_bind(prepared->prepared);
    break;
  default:
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC,
      "Unsupported statement type.");
    return NULL;
  }

  if (arguments && bind_arguments(stmt, arguments TSRMLS_CC) == FAILURE) {
    cass_statement_free(stmt);
    return NULL;
  }

  return stmt;
}

static CassBatch*
create_batch(cassandra_batch_statement* batch, CassConsistency consistency TSRMLS_DC)
{
  HashPosition pos;
  void** data;
  HashTable* statements = batch->statements;
  CassBatch* cass_batch = cass_batch_new(batch->batch_type);

  zend_hash_internal_pointer_reset_ex(statements, &pos);
  while (zend_hash_get_current_data_ex(statements, (void**)&data, &pos) == SUCCESS) {
    cassandra_batch_statement_entry* entry = *((cassandra_batch_statement_entry**)data);
    cassandra_statement* statement =
        (cassandra_statement*) zend_object_store_get_object(entry->statement TSRMLS_CC);
    HashTable* arguments
        = entry->arguments ? Z_ARRVAL_P(entry->arguments) : NULL;
    CassStatement* stmt = create_statement(statement, arguments TSRMLS_CC);
    if (!stmt) {
      cass_batch_free(cass_batch);
      return NULL;
    }
    cass_batch_add_statement(cass_batch, stmt);
    zend_hash_move_forward_ex(statements, &pos);
  }

  CassError rc = cass_batch_set_consistency(cass_batch, consistency);

  if (rc != CASS_OK) {
    cass_batch_free(cass_batch);
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
                            "%s", cass_error_desc(rc));
    return NULL;
  }

  return cass_batch;
}

static CassStatement*
create_single(cassandra_statement* statement, HashTable* arguments,
              CassConsistency consistency, long serial_consistency,
              int page_size TSRMLS_DC)
{
  CassStatement* stmt = create_statement(statement, arguments TSRMLS_CC);
  if (!stmt)
    return NULL;

  CassError rc = cass_statement_set_consistency(stmt, consistency);

  if (rc == CASS_OK && serial_consistency >= 0)
    rc = cass_statement_set_serial_consistency(stmt, serial_consistency);

  if (rc == CASS_OK && page_size >= 0)
    rc = cass_statement_set_paging_size(stmt, page_size);

  if (rc != CASS_OK) {
    cass_statement_free(stmt);
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
                            "%s", cass_error_desc(rc));
    return NULL;
  }

  return stmt;
}

PHP_METHOD(DefaultSession, execute)
{
  zval *statement = NULL;
  zval *options = NULL;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  cassandra_session* self =
    (cassandra_session*) zend_object_store_get_object(getThis() TSRMLS_CC);

  cassandra_statement* stmt =
    (cassandra_statement*) zend_object_store_get_object(statement TSRMLS_CC);

  HashTable*      arguments          = NULL;
  CassConsistency consistency        = self->default_consistency;
  int             page_size          = self->default_page_size;
  zval*           timeout            = self->default_timeout;
  long            serial_consistency = -1;

  if (options) {
    if (!instanceof_function(Z_OBJCE_P(options), cassandra_execution_options_ce TSRMLS_CC)) {
      INVALID_ARGUMENT(options, "an instance of Cassandra\\ExecutionOptions or null");
    }

    cassandra_execution_options* opts =
      (cassandra_execution_options*) zend_object_store_get_object(options TSRMLS_CC);

    if (opts->arguments)
      arguments = Z_ARRVAL_P(opts->arguments);

    if (opts->consistency >= 0)
      consistency = (CassConsistency) opts->consistency;

    if (opts->page_size >= 0)
      page_size = opts->page_size;

    if (opts->timeout)
      timeout = opts->timeout;

    if (opts->serial_consistency >= 0)
      serial_consistency = opts->serial_consistency;
  }

  CassFuture*    future;
  CassStatement* single = NULL;
  CassBatch*     batch  = NULL;

  switch (stmt->type) {
    case CASSANDRA_SIMPLE_STATEMENT:
    case CASSANDRA_PREPARED_STATEMENT:
      single = create_single(stmt, arguments, consistency,
                             serial_consistency, page_size TSRMLS_CC);

      if (!single)
        return;

      future = cass_session_execute(self->session, single);
      break;
    case CASSANDRA_BATCH_STATEMENT:
      batch = create_batch((cassandra_batch_statement*) stmt, consistency TSRMLS_CC);

      if (!batch)
        return;

      future = cass_session_execute_batch(self->session, batch);
      break;
    default:
      INVALID_ARGUMENT(statement,
        "an instance of Cassandra\\SimpleStatement, " \
        "Cassandra\\PreparedStatement or Cassandra\\BatchStatement"
      );
      return;
  }

  do {
    if (php_cassandra_future_wait_timed(future, timeout TSRMLS_CC) == FAILURE ||
        php_cassandra_future_is_error(future TSRMLS_CC) == FAILURE)
      break;

    const CassResult* result = cass_future_get_result(future);

    if (!result) {
      zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC,
                              "Future doesn't contain a result.");
      break;
    }

    object_init_ex(return_value, cassandra_rows_ce);
    cassandra_rows* rows =
      (cassandra_rows*) zend_object_store_get_object(return_value TSRMLS_CC);

    if (php_cassandra_get_result(result, &rows->rows TSRMLS_CC) == FAILURE) {
      cass_result_free(result);
      break;
    }

    if (single) {
      Z_ADDREF_P(getThis());

      rows->statement = php_cassandra_new_ref(single);
      rows->session   = getThis();
      rows->result    = result;
      cass_future_free(future);
      return;
    }

    cass_result_free(result);
  } while (0);

  if (batch)
    cass_batch_free(batch);

  if (single)
    cass_statement_free(single);

  cass_future_free(future);
}

PHP_METHOD(DefaultSession, executeAsync)
{
  zval *statement = NULL;
  zval *options = NULL;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|z", &statement,
                           cassandra_statement_ce, &options) == FAILURE) {
    return;
  }

  cassandra_session* self =
    (cassandra_session*) zend_object_store_get_object(getThis() TSRMLS_CC);

  cassandra_statement* stmt =
    (cassandra_statement*) zend_object_store_get_object(statement TSRMLS_CC);

  HashTable*      arguments          = NULL;
  CassConsistency consistency        = self->default_consistency;
  int             page_size          = self->default_page_size;
  long            serial_consistency = -1;

  if (options) {
    if (!instanceof_function(Z_OBJCE_P(options), cassandra_execution_options_ce TSRMLS_CC)) {
      INVALID_ARGUMENT(options, "an instance of Cassandra\\ExecutionOptions or null");
    }

    cassandra_execution_options* opts =
      (cassandra_execution_options*) zend_object_store_get_object(options TSRMLS_CC);

    if (opts->arguments)
      arguments = Z_ARRVAL_P(opts->arguments);

    if (opts->consistency >= 0)
      consistency = (CassConsistency) opts->consistency;

    if (opts->page_size >= 0)
      page_size = opts->page_size;

    if (opts->serial_consistency >= 0)
      serial_consistency = opts->serial_consistency;
  }

  object_init_ex(return_value, cassandra_future_rows_ce);
  cassandra_future_rows* future_rows =
    (cassandra_future_rows*) zend_object_store_get_object(return_value TSRMLS_CC);

  CassStatement* single = NULL;
  CassBatch*     batch  = NULL;

  switch (stmt->type) {
    case CASSANDRA_SIMPLE_STATEMENT:
    case CASSANDRA_PREPARED_STATEMENT:
      single = create_single(stmt, arguments, consistency,
                             serial_consistency, page_size TSRMLS_CC);

      if (!single)
        return;

      Z_ADDREF_P(getThis());

      future_rows->statement = php_cassandra_new_ref(single);
      future_rows->session   = getThis();
      future_rows->future    = cass_session_execute(self->session, single);
      break;
    case CASSANDRA_BATCH_STATEMENT:
      batch = create_batch((cassandra_batch_statement*) stmt, consistency TSRMLS_CC);

      if (!batch)
        return;

      future_rows->future = cass_session_execute_batch(self->session, batch);
      break;
    default:
      INVALID_ARGUMENT(statement,
        "an instance of one of Cassandra\\SimpleStatement, " \
        "Cassandra\\PreparedStatement or Cassandra\\BatchStatement"
      );
      return;
  }
}

PHP_METHOD(DefaultSession, prepare)
{
  zval *cql = NULL;
  zval *options = NULL;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &cql, &options) == FAILURE) {
    return;
  }

  cassandra_session* self =
    (cassandra_session*)zend_object_store_get_object(getThis() TSRMLS_CC);

  cassandra_execution_options* internal_options = NULL;
  if (options) {
    if (Z_TYPE_P(options) != IS_ARRAY)  {
      INVALID_ARGUMENT(options, "an array");
    }
    internal_options = (cassandra_execution_options*)zend_object_store_get_object(options TSRMLS_CC);
  }

  CassFuture* future = cass_session_prepare_n(self->session, Z_STRVAL_P(cql), Z_STRLEN_P(cql));

  zval* timeout = internal_options ? internal_options->timeout : NULL;
  if (php_cassandra_future_wait_timed(future, timeout TSRMLS_CC) == FAILURE) {
    cass_future_free(future);
    return;
  }

  if (php_cassandra_future_is_error(future TSRMLS_CC) == FAILURE) {
    cass_future_free(future);
    return;
  }

  object_init_ex(return_value, cassandra_prepared_statement_ce);
  cassandra_prepared_statement* prepared_statement =
      (cassandra_prepared_statement*)zend_object_store_get_object(return_value TSRMLS_CC);

  prepared_statement->prepared = cass_future_get_prepared(future);

  cass_future_free(future);
}

PHP_METHOD(DefaultSession, prepareAsync)
{
  zval *cql = NULL;
  zval *options = NULL;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &cql, &options) == FAILURE) {
    return;
  }

  cassandra_session* self =
    (cassandra_session*)zend_object_store_get_object(getThis() TSRMLS_CC);

  cassandra_execution_options* internal_options = NULL;
  if (options) {
    if (Z_TYPE_P(options) != IS_ARRAY)  {
      INVALID_ARGUMENT(options, "an array");
    }
    internal_options = (cassandra_execution_options*)zend_object_store_get_object(options TSRMLS_CC);
  }

  CassFuture* future = cass_session_prepare_n(self->session, Z_STRVAL_P(cql), Z_STRLEN_P(cql));

  object_init_ex(return_value, cassandra_future_prepared_statement_ce);
  cassandra_future_prepared_statement* future_prepared =
   (cassandra_future_prepared_statement*)zend_object_store_get_object(return_value TSRMLS_CC);

  future_prepared->future = future;
}

PHP_METHOD(DefaultSession, close)
{
  zval* timeout = NULL;

  cassandra_session* self =
    (cassandra_session*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  if (self->persist)
    return;

  CassFuture* future = cass_session_close(self->session);

  if (php_cassandra_future_wait_timed(future, timeout TSRMLS_CC) == SUCCESS)
    php_cassandra_future_is_error(future TSRMLS_CC);

  cass_future_free(future);
}

PHP_METHOD(DefaultSession, closeAsync)
{
  cassandra_session* self =
    (cassandra_session*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->persist) {
    object_init_ex(return_value, cassandra_future_value_ce);
    return;
  }

  if(zend_parse_parameters_none() == FAILURE)
    return;


  object_init_ex(return_value, cassandra_future_close_ce);
  cassandra_future_close* future =
    (cassandra_future_close*) zend_object_store_get_object(return_value TSRMLS_CC);

  future->future = cass_session_close(self->session);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, statement, Cassandra\\Statement, 0)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_prepare, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, cql)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\ExecutionOptions, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_default_session_methods[] = {
  PHP_ME(DefaultSession, execute, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, executeAsync, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, prepare, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, prepareAsync, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, close, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, closeAsync, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_default_session_handlers;

static HashTable*
php_cassandra_default_session_properties(zval *object TSRMLS_DC)
{
  cassandra_session* session = (cassandra_session*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*         props   = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_default_session_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_default_session_free(void *object TSRMLS_DC)
{
  cassandra_session* session = (cassandra_session*) object;

  zend_object_std_dtor(&session->zval TSRMLS_CC);

  if (!session->persist && session->session) {
    cass_session_free(session->session);
  }

  efree(session);
}

static zend_object_value
php_cassandra_default_session_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_session *session;

  session = (cassandra_session*) ecalloc(1, sizeof(cassandra_session));

  zend_object_std_init(&session->zval, class_type TSRMLS_CC);
  object_properties_init(&session->zval, class_type);

  session->session             = NULL;
  session->persist             = 0;
  session->default_consistency = CASS_CONSISTENCY_ONE;
  session->default_page_size   = 10000;
  session->default_timeout     = NULL;

  retval.handle   = zend_objects_store_put(session,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_default_session_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_default_session_handlers;

  return retval;
}

void cassandra_define_DefaultSession(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\DefaultSession", cassandra_default_session_methods);
  cassandra_default_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_default_session_ce TSRMLS_CC, 1, cassandra_session_ce);
  cassandra_default_session_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_default_session_ce->create_object = php_cassandra_default_session_new;

  memcpy(&cassandra_default_session_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_default_session_handlers.get_properties  = php_cassandra_default_session_properties;
  cassandra_default_session_handlers.compare_objects = php_cassandra_default_session_compare;
}
