#include "php_cassandra.h"
#include "util/bytes.h"
#include "util/future.h"
#include "util/math.h"
#include "util/collections.h"
#include "types/collection.h"
#include "types/map.h"
#include "types/set.h"

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
bind_argument_by_index(CassStatement* statement, zval* index, zval* value)
{
  if (Z_TYPE_P(value) == IS_NULL)
    CHECK_RESULT(cass_statement_bind_null(statement, (cass_size_t) Z_LVAL_P(index)));

  if (Z_TYPE_P(value) == IS_STRING)
    CHECK_RESULT(cass_statement_bind_string(statement, (cass_size_t) Z_LVAL_P(index),
                                            cass_string_init2(Z_STRVAL_P(value), Z_STRLEN_P(value))));

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(cass_statement_bind_double(statement, (cass_size_t) Z_LVAL_P(index), Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(cass_statement_bind_int32(statement, (cass_size_t) Z_LVAL_P(index), Z_LVAL_P(value)));

  if (Z_TYPE_P(value) == IS_BOOL)
    CHECK_RESULT(cass_statement_bind_bool(statement, (cass_size_t) Z_LVAL_P(index), Z_BVAL_P(value)));

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Float TSRMLS_CC)) {
      cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_float(statement, (cass_size_t) Z_LVAL_P(index), float_number->value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Bigint TSRMLS_CC)) {
      cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_int64(statement, (cass_size_t) Z_LVAL_P(index), bigint->value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Timestamp TSRMLS_CC)) {
      cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_int64(statement, (cass_size_t) Z_LVAL_P(index), timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Blob TSRMLS_CC)) {
      cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_bytes(statement, (cass_size_t) Z_LVAL_P(index), cass_bytes_init(blob->data, blob->size)));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Varint TSRMLS_CC)) {
      cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
      cass_size_t size;
      cass_byte_t* data;
      data = (cass_byte_t*) export_twos_complement(varint->value, &size);
      CassError rc = cass_statement_bind_bytes(statement, (cass_size_t) Z_LVAL_P(index), cass_bytes_init(data, size));
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Decimal TSRMLS_CC)) {
      cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
      cass_size_t size;
      cass_byte_t* data;
      data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
      CassError rc = cass_statement_bind_decimal(statement, (cass_size_t) Z_LVAL_P(index), cass_decimal_init(decimal->scale, cass_bytes_init(data, size)));
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Uuid TSRMLS_CC)) {
      cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_uuid(statement, (cass_size_t) Z_LVAL_P(index), uuid->uuid));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Inet TSRMLS_CC)) {
      cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_inet(statement, (cass_size_t) Z_LVAL_P(index), inet->inet));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Set TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_set* set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_set(set, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection(statement, (cass_size_t) Z_LVAL_P(index), collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Map TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_map* map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_map(map, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection(statement, (cass_size_t) Z_LVAL_P(index), collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Collection TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_collection* coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_collection(coll, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection(statement, (cass_size_t) Z_LVAL_P(index), collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int
bind_argument_by_name(CassStatement* statement, zval* name, zval* value)
{
  if (Z_TYPE_P(value) == IS_NULL) {
    CHECK_RESULT(cass_statement_bind_null_by_name(statement, Z_STRVAL_P(name)));
  }

  if (Z_TYPE_P(value) == IS_STRING)
    CHECK_RESULT(cass_statement_bind_string_by_name(statement, Z_STRVAL_P(name),
                                                    cass_string_init2(Z_STRVAL_P(value), Z_STRLEN_P(value))));

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(cass_statement_bind_double_by_name(statement, Z_STRVAL_P(name), Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(cass_statement_bind_int32_by_name(statement, Z_STRVAL_P(name), Z_LVAL_P(value)));

  if (Z_TYPE_P(value) == IS_BOOL)
    CHECK_RESULT(cass_statement_bind_bool_by_name(statement, Z_STRVAL_P(name), Z_BVAL_P(value)));

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Float TSRMLS_CC)) {
      cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_float_by_name(statement, Z_STRVAL_P(name), float_number->value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Bigint TSRMLS_CC)) {
      cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, Z_STRVAL_P(name), bigint->value));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Timestamp TSRMLS_CC)) {
      cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, Z_STRVAL_P(name), timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Blob TSRMLS_CC)) {
      cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_bytes_by_name(statement, Z_STRVAL_P(name), cass_bytes_init(blob->data, blob->size)));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Varint TSRMLS_CC)) {
      cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
      cass_size_t size;
      cass_byte_t* data;
      data = (cass_byte_t*) export_twos_complement(varint->value, &size);
      CassError rc = cass_statement_bind_bytes_by_name(statement, Z_STRVAL_P(name), cass_bytes_init(data, size));
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Decimal TSRMLS_CC)) {
      cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
      cass_size_t size;
      cass_byte_t* data;
      data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
      CassError rc = cass_statement_bind_decimal_by_name(statement, Z_STRVAL_P(name), cass_decimal_init(decimal->scale, cass_bytes_init(data, size)));
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Uuid TSRMLS_CC)) {
      cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_uuid_by_name(statement, Z_STRVAL_P(name), uuid->uuid));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Inet TSRMLS_CC)) {
      cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
      CHECK_RESULT(cass_statement_bind_inet_by_name(statement, Z_STRVAL_P(name), inet->inet));
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Set TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_set* set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_set(set, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection_by_name(statement, Z_STRVAL_P(name), collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Map TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_map* map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_map(map, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection_by_name(statement, Z_STRVAL_P(name), collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), cassandra_ce_Collection TSRMLS_CC)) {
      CassCollection* collection;
      cassandra_collection* coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
      if (!php_cassandra_collection_from_collection(coll, &collection TSRMLS_CC))
        return FAILURE;

      CassError rc = cass_statement_bind_collection_by_name(statement, Z_STRVAL_P(name), collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int
bind_argument(CassStatement* statement, zval* key, zval* value)
{
  if (Z_TYPE_P(key) == IS_LONG) {
    return bind_argument_by_index(statement, key, value);
  } else if (Z_TYPE_P(key) == IS_STRING) {
    return bind_argument_by_name(statement, key, value);
  }
  return FAILURE;
}

static int
bind_arguments(CassStatement* statement, HashTable* arguments)
{
  HashPosition pos;
  zval* key;
  zval** value;
  zend_hash_internal_pointer_reset_ex(arguments, &pos);
  while (zend_hash_get_current_data_ex(arguments, (void**)&value, &pos) == SUCCESS) {
    zend_hash_get_current_key_zval_ex(arguments, key, &pos);
    if (bind_argument(statement, key, *value) == FAILURE) {
      return FAILURE;
    }
    zend_hash_move_forward_ex(arguments, &pos);
  }

  return SUCCESS;
}

static int set_options(CassStatement* statement,
                       cassandra_execution_options* options)
{
  if (options) {
    if (options->arguments) {
      if (bind_arguments(statement, Z_ARRVAL_P(options->arguments)) == FAILURE) {
        cass_statement_free(statement);
        return FAILURE;
      }
    }

    if (options->consistency >= 0)
      ASSERT_SUCCESS_VALUE(cass_statement_set_consistency(statement, (CassConsistency)options->consistency), FAILURE);

    if (options->serial_consistency >= 0)
      ASSERT_SUCCESS_VALUE(cass_statement_set_serial_consistency(statement, (CassConsistency)options->serial_consistency), FAILURE);

    if (options->page_size >= 0)
      ASSERT_SUCCESS_VALUE(cass_statement_set_paging_size(statement, options->page_size), FAILURE);
  }

  return SUCCESS;
}

static CassFuture*
execute_simple(CassSession* session,
               cassandra_simple_statement* simple,
               cassandra_execution_options* options)
{
  zend_uint count = 0;

  if (options && options->arguments) {
    count = Z_ARRVAL_P(options->arguments)->nNumOfElements;
  }

  CassStatement* statement = cass_statement_new(cass_string_init(simple->cql), count);

  if (set_options(statement, options) == FAILURE) {
    return NULL;
  }

  CassFuture* future = cass_session_execute(session, statement);
  cass_statement_free(statement);
  return future;
}

static CassFuture*
execute_prepared(CassSession* session,
               cassandra_prepared_statement* prepared,
               cassandra_execution_options* options)
{
  CassStatement* statement = cass_prepared_bind(prepared->prepared);

  if (set_options(statement, options) == FAILURE) {
    return NULL;
  }

  CassFuture* future = cass_session_execute(session, statement);
  cass_statement_free(statement);
  return future;
}

static CassFuture*
execute_batch(CassSession* session,
              cassandra_batch_statement* batch)
{
  return NULL;
}

static CassFuture* execute_async(CassSession* session,
                                 cassandra_statement* statement,
                                 cassandra_execution_options* options)
{
  CassFuture* future = NULL;
  switch(statement->type) {
  case CASSANDRA_SIMPLE_STATEMENT:
    future = execute_simple(session, (cassandra_simple_statement*)statement, options);
    break;
  case CASSANDRA_PREPARED_STATEMENT:
    future  = execute_prepared(session, (cassandra_prepared_statement*)statement, options);
    break;
  case CASSANDRA_BATCH_STATEMENT:
    future  = execute_batch(session, (cassandra_batch_statement*)statement);
    break;
  default:
    ZEND_ASSERT(0);
  }
  return future;
}

PHP_METHOD(DefaultSession, execute)
{
  zval *statement = NULL;
  zval *options = NULL;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  cassandra_session* self =
    (cassandra_session*)zend_object_store_get_object(getThis() TSRMLS_CC);

  cassandra_statement* internal_statement
    = (cassandra_statement*)zend_object_store_get_object(statement TSRMLS_CC);

  cassandra_execution_options* internal_options = NULL;
  if (options) {
    internal_options = (cassandra_execution_options*)zend_object_store_get_object(options TSRMLS_CC);
  }

  CassFuture* future = execute_async(self->session, internal_statement, internal_options);
  if (!future) return;

  zval* timeout = internal_options ? internal_options->timeout : NULL;
  if (php_cassandra_future_wait_timed(future, timeout) == FAILURE) {
    cass_future_free(future);
    return;
  }

  if (php_cassandra_future_is_error(future) == FAILURE) {
    cass_future_free(future);
    return;
  }

  cass_future_free(future);

  /* TODO: Handle rows */
  RETURN_TRUE;
}

PHP_METHOD(DefaultSession, executeAsync)
{
  zval *statement = NULL;
  zval *options = NULL;

  if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  cassandra_session* self =
    (cassandra_session*) zend_object_store_get_object(getThis() TSRMLS_CC);

  cassandra_statement* internal_statement
    = (cassandra_statement*) zend_object_store_get_object(statement TSRMLS_CC);

  cassandra_execution_options* internal_options = NULL;
  if (options) {
    internal_options = (cassandra_execution_options*) zend_object_store_get_object(options TSRMLS_CC);
  }

  CassFuture* future = execute_async(self->session, internal_statement, internal_options);
  if (!future) return;

  object_init_ex(return_value, cassandra_future_rows_ce);
  cassandra_future_rows* future_rows =
   (cassandra_future_rows*) zend_object_store_get_object(return_value TSRMLS_CC);

  future_rows->future = future;
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

  CassFuture* future = cass_session_prepare(self->session,
                                            cass_string_init2(Z_STRVAL_P(cql),
                                                              Z_STRLEN_P(cql)));

  zval* timeout = internal_options ? internal_options->timeout : NULL;
  if (php_cassandra_future_wait_timed(future, timeout) == FAILURE) {
    cass_future_free(future);
    return;
  }

  if (php_cassandra_future_is_error(future) == FAILURE) {
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

  CassFuture* future = cass_session_prepare(self->session,
                                            cass_string_init2(Z_STRVAL_P(cql),
                                                              Z_STRLEN_P(cql)));

  object_init_ex(return_value, cassandra_future_prepared_statement_ce);
  cassandra_future_prepared_statement* future_prepared =
   (cassandra_future_prepared_statement*)zend_object_store_get_object(return_value TSRMLS_CC);

  future_prepared->future = future;
}

PHP_METHOD(DefaultSession, close)
{

}

PHP_METHOD(DefaultSession, closeAsync)
{

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

  if (!session->persist) {
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

  session->session  = NULL;
  session->persist  = 0;

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
