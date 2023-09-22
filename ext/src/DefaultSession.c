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
#include "php_driver_globals.h"
#include "php_driver_types.h"
#include "util/bytes.h"
#include "util/future.h"
#include "util/result.h"
#include "util/ref.h"
#include "util/math.h"
#include "util/collections.h"
#include "ExecutionOptions.h"

zend_class_entry *php_driver_default_session_ce = NULL;

#define CHECK_RESULT(rc) \
{ \
  ASSERT_SUCCESS_VALUE(rc, FAILURE) \
  return SUCCESS; \
}

static void
free_result(void *result)
{
  cass_result_free((CassResult *) result);
}

static void
free_statement(void *statement)
{
  cass_statement_free((CassStatement *) statement);
}

static void
free_schema(void *schema)
{
  cass_schema_meta_free((CassSchemaMeta *) schema);
}

static int
bind_argument_by_index(CassStatement *statement, size_t index, zval *value TSRMLS_DC)
{
  if (Z_TYPE_P(value) == IS_NULL)
    CHECK_RESULT(cass_statement_bind_null(statement, index));

  if (Z_TYPE_P(value) == IS_STRING)
    CHECK_RESULT(cass_statement_bind_string(statement, index, Z_STRVAL_P(value)));

  if (Z_TYPE_P(value) == IS_DOUBLE)
    CHECK_RESULT(cass_statement_bind_double(statement, index, Z_DVAL_P(value)));

  if (Z_TYPE_P(value) == IS_LONG)
    CHECK_RESULT(cass_statement_bind_int32(statement, index, Z_LVAL_P(value)));

  if (PHP5TO7_ZVAL_IS_TRUE_P(value))
    CHECK_RESULT(cass_statement_bind_bool(statement, index, cass_true));

  if (PHP5TO7_ZVAL_IS_FALSE_P(value))
    CHECK_RESULT(cass_statement_bind_bool(statement, index, cass_false));

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), php_driver_float_ce TSRMLS_CC)) {
      php_driver_numeric *float_number = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_float(statement, index, float_number->data.floating.value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_bigint_ce TSRMLS_CC)) {
      php_driver_numeric *bigint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int64(statement, index, bigint->data.bigint.value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_smallint_ce TSRMLS_CC)) {
      php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int16(statement, index, smallint->data.smallint.value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tinyint_ce TSRMLS_CC)) {
      php_driver_numeric *tinyint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int8(statement, index, tinyint->data.tinyint.value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_timestamp_ce TSRMLS_CC)) {
      php_driver_timestamp *timestamp = PHP_DRIVER_GET_TIMESTAMP(value);
      CHECK_RESULT(cass_statement_bind_int64(statement, index, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_date_ce TSRMLS_CC)) {
      php_driver_date *date = PHP_DRIVER_GET_DATE(value);
      CHECK_RESULT(cass_statement_bind_uint32(statement, index, date->date));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_time_ce TSRMLS_CC)) {
      php_driver_time *time = PHP_DRIVER_GET_TIME(value);
      CHECK_RESULT(cass_statement_bind_int64(statement, index, time->time));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_blob_ce TSRMLS_CC)) {
      php_driver_blob *blob = PHP_DRIVER_GET_BLOB(value);
      CHECK_RESULT(cass_statement_bind_bytes(statement, index, blob->data, blob->size));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_varint_ce TSRMLS_CC)) {
      php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(value);
      size_t size;
      cass_byte_t *data = export_twos_complement(varint->data.varint.value, &size);
      CassError rc = cass_statement_bind_bytes(statement, index, data, size);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_decimal_ce TSRMLS_CC)) {
      php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(value);
      size_t size;
      cass_byte_t *data = (cass_byte_t *) export_twos_complement(decimal->data.decimal.value, &size);
      CassError rc = cass_statement_bind_decimal(statement, index, data, size, decimal->data.decimal.scale);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_uuid_interface_ce TSRMLS_CC)) {
      php_driver_uuid *uuid = PHP_DRIVER_GET_UUID(value);
      CHECK_RESULT(cass_statement_bind_uuid(statement, index, uuid->uuid));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_inet_ce TSRMLS_CC)) {
      php_driver_inet *inet = PHP_DRIVER_GET_INET(value);
      CHECK_RESULT(cass_statement_bind_inet(statement, index, inet->inet));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_duration_ce TSRMLS_CC)) {
      php_driver_duration *duration = PHP_DRIVER_GET_DURATION(value);
      CHECK_RESULT(cass_statement_bind_duration(statement,
                                                index,
                                                duration->months, duration->days, duration->nanos));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_set_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_set *set = PHP_DRIVER_GET_SET(value);
      if (!php_driver_collection_from_set(set, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_map_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_map *map = PHP_DRIVER_GET_MAP(value);
      if (!php_driver_collection_from_map(map, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_collection_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_collection *coll = PHP_DRIVER_GET_COLLECTION(value);
      if (!php_driver_collection_from_collection(coll, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection(statement, index, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tuple_ce TSRMLS_CC)) {
      CassError rc;
      CassTuple *tup;
      php_driver_tuple *tuple = PHP_DRIVER_GET_TUPLE(value);
      if (!php_driver_tuple_from_tuple(tuple, &tup TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_tuple(statement, index, tup);
      cass_tuple_free(tup);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_user_type_value_ce TSRMLS_CC)) {
      CassError rc;
      CassUserType *ut;
      php_driver_user_type_value *user_type_value = PHP_DRIVER_GET_USER_TYPE_VALUE(value);
      if (!php_driver_user_type_from_user_type_value(user_type_value, &ut TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_user_type(statement, index, ut);
      cass_user_type_free(ut);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int
bind_argument_by_name(CassStatement *statement, const char *name,
                      zval *value TSRMLS_DC)
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

  if (PHP5TO7_ZVAL_IS_TRUE_P(value))
    CHECK_RESULT(cass_statement_bind_bool_by_name(statement, name, cass_true));

  if (PHP5TO7_ZVAL_IS_FALSE_P(value))
    CHECK_RESULT(cass_statement_bind_bool_by_name(statement, name, cass_false));

  if (Z_TYPE_P(value) == IS_OBJECT) {
    if (instanceof_function(Z_OBJCE_P(value), php_driver_float_ce TSRMLS_CC)) {
      php_driver_numeric *float_number = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_float_by_name(statement, name, float_number->data.floating.value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_bigint_ce TSRMLS_CC)) {
      php_driver_numeric *bigint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, name, bigint->data.bigint.value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_smallint_ce TSRMLS_CC)) {
      php_driver_numeric *smallint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int16_by_name(statement, name, smallint->data.smallint.value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tinyint_ce TSRMLS_CC)) {
      php_driver_numeric *tinyint = PHP_DRIVER_GET_NUMERIC(value);
      CHECK_RESULT(cass_statement_bind_int8_by_name(statement, name, tinyint->data.tinyint.value));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_timestamp_ce TSRMLS_CC)) {
      php_driver_timestamp *timestamp = PHP_DRIVER_GET_TIMESTAMP(value);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, name, timestamp->timestamp));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_date_ce TSRMLS_CC)) {
      php_driver_date *date = PHP_DRIVER_GET_DATE(value);
      CHECK_RESULT(cass_statement_bind_uint32_by_name(statement, name, date->date));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_time_ce TSRMLS_CC)) {
      php_driver_time *time = PHP_DRIVER_GET_TIME(value);
      CHECK_RESULT(cass_statement_bind_int64_by_name(statement, name, time->time));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_blob_ce TSRMLS_CC)) {
      php_driver_blob *blob = PHP_DRIVER_GET_BLOB(value);
      CHECK_RESULT(cass_statement_bind_bytes_by_name(statement, name, blob->data, blob->size));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_varint_ce TSRMLS_CC)) {
      php_driver_numeric *varint = PHP_DRIVER_GET_NUMERIC(value);
      size_t size;
      cass_byte_t *data = (cass_byte_t *) export_twos_complement(varint->data.varint.value, &size);
      CassError rc = cass_statement_bind_bytes_by_name(statement, name, data, size);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_decimal_ce TSRMLS_CC)) {
      php_driver_numeric *decimal = PHP_DRIVER_GET_NUMERIC(value);
      size_t size;
      cass_byte_t *data = (cass_byte_t *) export_twos_complement(decimal->data.decimal.value, &size);
      CassError rc = cass_statement_bind_decimal_by_name(statement, name, data, size, decimal->data.decimal.scale);
      free(data);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_uuid_interface_ce TSRMLS_CC)) {
      php_driver_uuid *uuid = PHP_DRIVER_GET_UUID(value);
      CHECK_RESULT(cass_statement_bind_uuid_by_name(statement, name, uuid->uuid));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_inet_ce TSRMLS_CC)) {
      php_driver_inet *inet = PHP_DRIVER_GET_INET(value);
      CHECK_RESULT(cass_statement_bind_inet_by_name(statement, name, inet->inet));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_duration_ce TSRMLS_CC)) {
      php_driver_duration *duration = PHP_DRIVER_GET_DURATION(value);
      CHECK_RESULT(cass_statement_bind_duration_by_name(statement,
                                                        name,
                                                        duration->months, duration->days, duration->nanos));
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_set_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_set *set = PHP_DRIVER_GET_SET(value);
      if (!php_driver_collection_from_set(set, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_map_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_map *map = PHP_DRIVER_GET_MAP(value);
      if (!php_driver_collection_from_map(map, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_collection_ce TSRMLS_CC)) {
      CassError rc;
      CassCollection *collection;
      php_driver_collection *coll = PHP_DRIVER_GET_COLLECTION(value);
      if (!php_driver_collection_from_collection(coll, &collection TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_collection_by_name(statement, name, collection);
      cass_collection_free(collection);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_tuple_ce TSRMLS_CC)) {
      CassError rc;
      CassTuple *tup;
      php_driver_tuple *tuple = PHP_DRIVER_GET_TUPLE(value);
      if (!php_driver_tuple_from_tuple(tuple, &tup TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_tuple_by_name(statement, name, tup);
      cass_tuple_free(tup);
      CHECK_RESULT(rc);
    }

    if (instanceof_function(Z_OBJCE_P(value), php_driver_user_type_value_ce TSRMLS_CC)) {
      CassError rc;
      CassUserType *ut;
      php_driver_user_type_value *user_type_value = PHP_DRIVER_GET_USER_TYPE_VALUE(value);
      if (!php_driver_user_type_from_user_type_value(user_type_value, &ut TSRMLS_CC))
        return FAILURE;

      rc = cass_statement_bind_user_type_by_name(statement, name, ut);
      cass_user_type_free(ut);
      CHECK_RESULT(rc);
    }
  }

  return FAILURE;
}

static int
bind_arguments(CassStatement *statement, HashTable *arguments TSRMLS_DC)
{
  int rc = SUCCESS;

  php5to7_zval *current;
  ulong num_key;

#if PHP_MAJOR_VERSION >= 7
  zend_string *key;
  ZEND_HASH_FOREACH_KEY_VAL(arguments, num_key, key, current) {
    if (key) {
      rc = bind_argument_by_name(statement, key->val,
                                 PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
#else
  char *str_key;
  uint str_len;
  PHP5TO7_ZEND_HASH_FOREACH_KEY_VAL(arguments, num_key, str_key, str_len, current) {
    if (str_key) {
      rc = bind_argument_by_name(statement, str_key,
                                 PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
#endif
    } else {
      rc = bind_argument_by_index(statement, num_key, PHP5TO7_ZVAL_MAYBE_DEREF(current) TSRMLS_CC);
    }
    if (rc == FAILURE) break;
  } PHP5TO7_ZEND_HASH_FOREACH_END(arguments);

  return rc;
}

static CassStatement *
create_statement(php_driver_statement *statement, HashTable *arguments TSRMLS_DC)
{
  CassStatement *stmt;
  uint32_t count;

  switch (statement->type) {
  case PHP_DRIVER_SIMPLE_STATEMENT:
    count  = 0;

    if (arguments)
      count = zend_hash_num_elements(arguments);

    stmt = cass_statement_new(statement->data.simple.cql, count);
    break;
  case PHP_DRIVER_PREPARED_STATEMENT:
    stmt = cass_prepared_bind(statement->data.prepared.prepared);
    break;
  default:
    zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC,
      "Unsupported statement type.");
    return NULL;
  }

  if (arguments && bind_arguments(stmt, arguments TSRMLS_CC) == FAILURE) {
    cass_statement_free(stmt);
    return NULL;
  }

  return stmt;
}

static CassBatch *
create_batch(php_driver_statement *batch,
             CassConsistency consistency,
             CassRetryPolicy *retry_policy,
             cass_int64_t timestamp TSRMLS_DC)
{
  CassBatch *cass_batch = cass_batch_new(batch->data.batch.type);
  CassError rc = CASS_OK;

  php5to7_zval *current;
  PHP5TO7_ZEND_HASH_FOREACH_VAL(&batch->data.batch.statements, current) {
    php_driver_statement *statement;
    php_driver_statement simple_statement;
    HashTable *arguments;
    CassStatement *stmt;

#if PHP_MAJOR_VERSION >= 7
    php_driver_batch_statement_entry *batch_statement_entry = (php_driver_batch_statement_entry *)Z_PTR_P(current);
#else
    php_driver_batch_statement_entry *batch_statement_entry = *((php_driver_batch_statement_entry **)current);
#endif

    if (PHP5TO7_Z_TYPE_MAYBE_P(batch_statement_entry->statement) == IS_STRING) {
      simple_statement.type = PHP_DRIVER_SIMPLE_STATEMENT;
      simple_statement.data.simple.cql = PHP5TO7_Z_STRVAL_MAYBE_P(batch_statement_entry->statement);
      statement = &simple_statement;
    } else {
      statement = PHP_DRIVER_GET_STATEMENT(PHP5TO7_ZVAL_MAYBE_P(batch_statement_entry->statement));
    }

    arguments = !PHP5TO7_ZVAL_IS_UNDEF(batch_statement_entry->arguments)
                ? Z_ARRVAL_P(PHP5TO7_ZVAL_MAYBE_P(batch_statement_entry->arguments))
                : NULL;

    stmt = create_statement(statement, arguments TSRMLS_CC);
    if (!stmt) {
      cass_batch_free(cass_batch);
      return NULL;
    }
    cass_batch_add_statement(cass_batch, stmt);
    cass_statement_free(stmt);
  } PHP5TO7_ZEND_HASH_FOREACH_END(&batch->data.batch.statements);

  rc = cass_batch_set_consistency(cass_batch, consistency);
  ASSERT_SUCCESS_BLOCK(rc,
    cass_batch_free(cass_batch);
    return NULL;
  )

  rc = cass_batch_set_retry_policy(cass_batch, retry_policy);
  ASSERT_SUCCESS_BLOCK(rc,
    cass_batch_free(cass_batch);
    return NULL;
  )

  rc = cass_batch_set_timestamp(cass_batch, timestamp);
  ASSERT_SUCCESS_BLOCK(rc,
    cass_batch_free(cass_batch);
    return NULL;
  )

  return cass_batch;
}

static CassStatement *
create_single(php_driver_statement *statement, HashTable *arguments,
              CassConsistency consistency, long serial_consistency,
              int page_size, const char* paging_state_token,
              size_t paging_state_token_size,
              CassRetryPolicy *retry_policy, cass_int64_t timestamp TSRMLS_DC)
{
  CassError rc = CASS_OK;
  CassStatement *stmt = create_statement(statement, arguments TSRMLS_CC);
  if (!stmt)
    return NULL;

  rc = cass_statement_set_consistency(stmt, consistency);

  if (rc == CASS_OK && serial_consistency >= 0)
    rc = cass_statement_set_serial_consistency(stmt, serial_consistency);

  if (rc == CASS_OK && page_size >= 0)
    rc = cass_statement_set_paging_size(stmt, page_size);

  if (rc == CASS_OK && paging_state_token) {
    rc = cass_statement_set_paging_state_token(stmt,
                                               paging_state_token,
                                               paging_state_token_size);
  }

  if (rc == CASS_OK && retry_policy)
    rc = cass_statement_set_retry_policy(stmt, retry_policy);

  if (rc == CASS_OK)
    rc = cass_statement_set_timestamp(stmt, timestamp);

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
  php_driver_session *self = NULL;
  php_driver_statement *stmt = NULL;
  php_driver_statement simple_statement;
  HashTable *arguments = NULL;
  CassConsistency consistency = PHP_DRIVER_DEFAULT_CONSISTENCY;
  int page_size = -1;
  char *paging_state_token = NULL;
  size_t paging_state_token_size = 0;
  zval *timeout = NULL;
  long serial_consistency = -1;
  CassRetryPolicy *retry_policy = NULL;
  cass_int64_t timestamp = INT64_MIN;
  php_driver_execution_options *opts = NULL;
  php_driver_execution_options local_opts;
  CassFuture *future = NULL;
  CassStatement *single = NULL;
  CassBatch *batch  = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  if (Z_TYPE_P(statement) == IS_STRING) {
    simple_statement.type = PHP_DRIVER_SIMPLE_STATEMENT;
    simple_statement.data.simple.cql = Z_STRVAL_P(statement);
    stmt = &simple_statement;
  } else if (Z_TYPE_P(statement) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(statement), php_driver_statement_ce TSRMLS_CC)) {
    stmt = PHP_DRIVER_GET_STATEMENT(statement);
  } else {
    INVALID_ARGUMENT(statement, "a string or an instance of " PHP_DRIVER_NAMESPACE "\\Statement");
  }

  consistency = self->default_consistency;
  page_size = self->default_page_size;
  timeout = PHP5TO7_ZVAL_MAYBE_P(self->default_timeout);

  if (options) {
    if (Z_TYPE_P(options) != IS_ARRAY &&
        (Z_TYPE_P(options) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(options), php_driver_execution_options_ce TSRMLS_CC))) {
      INVALID_ARGUMENT(options, "an instance of " PHP_DRIVER_NAMESPACE "\\ExecutionOptions or an array or null");
    }

    if (Z_TYPE_P(options) == IS_OBJECT) {
      opts = PHP_DRIVER_GET_EXECUTION_OPTIONS(options);
    } else {
      if (php_driver_execution_options_build_local_from_array(&local_opts, options TSRMLS_CC) == FAILURE) {
        return;
      }
      opts = &local_opts;
    }

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->arguments))
      arguments = PHP5TO7_Z_ARRVAL_MAYBE_P(opts->arguments);

    if (opts->consistency >= 0)
      consistency = (CassConsistency) opts->consistency;

    if (opts->page_size >= 0)
      page_size = opts->page_size;

    if (opts->paging_state_token) {
      paging_state_token = opts->paging_state_token;
      paging_state_token_size = opts->paging_state_token_size;
    }

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->timeout))
      timeout = PHP5TO7_ZVAL_MAYBE_P(opts->timeout);

    if (opts->serial_consistency >= 0)
      serial_consistency = opts->serial_consistency;

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->retry_policy))
      retry_policy = (PHP_DRIVER_GET_RETRY_POLICY(PHP5TO7_ZVAL_MAYBE_P(opts->retry_policy)))->policy;

    timestamp = opts->timestamp;
  }

  switch (stmt->type) {
    case PHP_DRIVER_SIMPLE_STATEMENT:
    case PHP_DRIVER_PREPARED_STATEMENT:
      single = create_single(stmt, arguments, consistency,
                             serial_consistency, page_size,
                             paging_state_token, paging_state_token_size,
                             retry_policy, timestamp TSRMLS_CC);

      if (!single)
        return;

      future = cass_session_execute((CassSession *) self->session->data, single);
      break;
    case PHP_DRIVER_BATCH_STATEMENT:
      batch = create_batch(stmt, consistency, retry_policy, timestamp TSRMLS_CC);

      if (!batch)
        return;

      future = cass_session_execute_batch((CassSession *) self->session->data, batch);
      break;
    default:
      INVALID_ARGUMENT(statement,
        "an instance of " PHP_DRIVER_NAMESPACE "\\SimpleStatement, " \
        PHP_DRIVER_NAMESPACE "\\PreparedStatement or " PHP_DRIVER_NAMESPACE "\\BatchStatement"
      );
      return;
  }

  do {
    const CassResult *result = NULL;
    php_driver_rows *rows = NULL;

    if (php_driver_future_wait_timed(future, timeout TSRMLS_CC) == FAILURE ||
        php_driver_future_is_error(future TSRMLS_CC) == FAILURE)
      break;

    result = cass_future_get_result(future);
    cass_future_free(future);

    if (!result) {
      zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC,
                              "Future doesn't contain a result.");
      break;
    }

    object_init_ex(return_value, php_driver_rows_ce);
    rows = PHP_DRIVER_GET_ROWS(return_value);

    if (php_driver_get_result(result, &rows->rows TSRMLS_CC) == FAILURE) {
      cass_result_free(result);
      break;
    }

    if (single && cass_result_has_more_pages(result)) {
      rows->statement = php_driver_new_ref(single, free_statement);
      rows->result    = php_driver_new_ref((void *)result, free_result);
      rows->session   = php_driver_add_ref(self->session);
      return;
    }

    cass_result_free(result);
  } while (0);

  if (batch)
    cass_batch_free(batch);

  if (single)
    cass_statement_free(single);
}

PHP_METHOD(DefaultSession, executeAsync)
{
  zval *statement = NULL;
  zval *options = NULL;
  php_driver_session *self = NULL;
  php_driver_statement *stmt = NULL;
  php_driver_statement simple_statement;
  HashTable *arguments = NULL;
  CassConsistency consistency = PHP_DRIVER_DEFAULT_CONSISTENCY;
  int page_size = -1;
  char *paging_state_token = NULL;
  size_t paging_state_token_size = 0;
  long serial_consistency = -1;
  CassRetryPolicy *retry_policy = NULL;
  cass_int64_t timestamp = INT64_MIN;
  php_driver_execution_options *opts = NULL;
  php_driver_execution_options local_opts;
  php_driver_future_rows *future_rows = NULL;
  CassStatement *single = NULL;
  CassBatch *batch  = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &statement, &options) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  if (Z_TYPE_P(statement) == IS_STRING) {
    simple_statement.type = PHP_DRIVER_SIMPLE_STATEMENT;
    simple_statement.data.simple.cql = Z_STRVAL_P(statement);
    stmt = &simple_statement;
  } else if (Z_TYPE_P(statement) == IS_OBJECT &&
             instanceof_function(Z_OBJCE_P(statement), php_driver_statement_ce TSRMLS_CC)) {
    stmt = PHP_DRIVER_GET_STATEMENT(statement);
  } else {
    INVALID_ARGUMENT(statement, "a string or an instance of " PHP_DRIVER_NAMESPACE "\\Statement");
  }

  consistency = self->default_consistency;
  page_size = self->default_page_size;

  if (options) {
    if (Z_TYPE_P(options) != IS_ARRAY &&
        (Z_TYPE_P(options) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(options), php_driver_execution_options_ce TSRMLS_CC))) {
      INVALID_ARGUMENT(options, "an instance of " PHP_DRIVER_NAMESPACE "\\ExecutionOptions or an array or null");
    }

    if (Z_TYPE_P(options) == IS_OBJECT) {
      opts = PHP_DRIVER_GET_EXECUTION_OPTIONS(options);
    } else {
      if (php_driver_execution_options_build_local_from_array(&local_opts, options TSRMLS_CC) == FAILURE) {
        return;
      }
      opts = &local_opts;
    }

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->arguments))
      arguments = PHP5TO7_Z_ARRVAL_MAYBE_P(opts->arguments);

    if (opts->consistency >= 0)
      consistency = (CassConsistency) opts->consistency;

    if (opts->page_size >= 0)
      page_size = opts->page_size;

    if (opts->paging_state_token) {
      paging_state_token = opts->paging_state_token;
      paging_state_token_size = opts->paging_state_token_size;
    }

    if (opts->serial_consistency >= 0)
      serial_consistency = opts->serial_consistency;

    if (!PHP5TO7_ZVAL_IS_UNDEF(opts->retry_policy))
      retry_policy = (PHP_DRIVER_GET_RETRY_POLICY(PHP5TO7_ZVAL_MAYBE_P(opts->retry_policy)))->policy;

    timestamp = opts->timestamp;
  }

  object_init_ex(return_value, php_driver_future_rows_ce);
  future_rows = PHP_DRIVER_GET_FUTURE_ROWS(return_value);

  switch (stmt->type) {
    case PHP_DRIVER_SIMPLE_STATEMENT:
    case PHP_DRIVER_PREPARED_STATEMENT:
      single = create_single(stmt, arguments, consistency,
                             serial_consistency, page_size,
                             paging_state_token, paging_state_token_size,
                             retry_policy, timestamp TSRMLS_CC);

      if (!single)
        return;

      future_rows->statement = php_driver_new_ref(single, free_statement);
      future_rows->future    = cass_session_execute((CassSession *) self->session->data, single);
      future_rows->session   = php_driver_add_ref(self->session);
      break;
    case PHP_DRIVER_BATCH_STATEMENT:
      batch = create_batch(stmt, consistency, retry_policy, timestamp TSRMLS_CC);

      if (!batch)
        return;

      future_rows->future = cass_session_execute_batch((CassSession *) self->session->data, batch);
      cass_batch_free(batch);
      break;
    default:
      INVALID_ARGUMENT(statement,
        "an instance of " PHP_DRIVER_NAMESPACE "\\SimpleStatement, " \
        PHP_DRIVER_NAMESPACE "\\PreparedStatement or " PHP_DRIVER_NAMESPACE "\\BatchStatement"
      );
      return;
  }
}

static void
free_prepared_statement(void *future)
{
    cass_future_free((CassFuture*) future);
}

PHP_METHOD(DefaultSession, prepare)
{
  zval *cql = NULL;
  zval *options = NULL;
  char *hash_key = NULL;
  php5to7_size hash_key_len = 0;
  php_driver_session *self = NULL;
  php_driver_execution_options *opts = NULL;
  php_driver_execution_options local_opts;
  CassFuture *future = NULL;
  zval *timeout = NULL;
  php_driver_statement *prepared_statement = NULL;
  php_driver_pprepared_statement *pprepared_statement = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &cql, &options) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  if (options) {
    if (Z_TYPE_P(options) != IS_ARRAY &&
        (Z_TYPE_P(options) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(options), php_driver_execution_options_ce TSRMLS_CC))) {
      INVALID_ARGUMENT(options, "an instance of " PHP_DRIVER_NAMESPACE "\\ExecutionOptions or an array or null");
    }

    if (Z_TYPE_P(options) == IS_OBJECT) {
      opts = PHP_DRIVER_GET_EXECUTION_OPTIONS(options);
    } else {
      if (php_driver_execution_options_build_local_from_array(&local_opts, options TSRMLS_CC) == FAILURE) {
        return;
      }
      opts = &local_opts;
    }
    timeout = PHP5TO7_ZVAL_MAYBE_P(opts->timeout);
  }

  if (self->persist) {
    php5to7_zend_resource_le *le;

    spprintf(&hash_key, 0, "%s%s", self->hash_key, Z_STRVAL_P(cql));
    hash_key_len = spprintf(&hash_key, 0, "%s:prepared_statement:%s",
                            hash_key, SAFE_STR(self->keyspace));

    if (PHP5TO7_ZEND_HASH_FIND(&EG(persistent_list), hash_key, hash_key_len + 1, le) &&
        Z_RES_P(le)->type == php_le_php_driver_prepared_statement()) {
      pprepared_statement = (php_driver_pprepared_statement *) Z_RES_P(le)->ptr;
      object_init_ex(return_value, php_driver_prepared_statement_ce);
      prepared_statement = PHP_DRIVER_GET_STATEMENT(return_value);
      prepared_statement->data.prepared.prepared = cass_future_get_prepared(pprepared_statement->future);
      self->session = php_driver_add_ref(pprepared_statement->ref);
      future = pprepared_statement->future;
    }
  }

  if (future == NULL) {
    php5to7_zend_resource_le resource;

    future = cass_session_prepare_n((CassSession *)self->session->data,
                                    Z_STRVAL_P(cql), Z_STRLEN_P(cql));

    if (php_driver_future_wait_timed(future, timeout TSRMLS_CC) == SUCCESS &&
            php_driver_future_is_error(future TSRMLS_CC) == SUCCESS) {
      object_init_ex(return_value, php_driver_prepared_statement_ce);
      prepared_statement = PHP_DRIVER_GET_STATEMENT(return_value);
      prepared_statement->data.prepared.prepared = cass_future_get_prepared(future);

      if (self->persist) {
          pprepared_statement = (php_driver_pprepared_statement *) pecalloc(1, sizeof(php_driver_pprepared_statement), 1);
          pprepared_statement->ref = php_driver_new_peref(future, free_prepared_statement, 1);
          pprepared_statement->ref = php_driver_add_ref(self->session);
          pprepared_statement->future = future;

          #if PHP_MAJOR_VERSION >= 7
                ZVAL_NEW_PERSISTENT_RES(&resource, 0, pprepared_statement, php_le_php_driver_prepared_statement());
                PHP5TO7_ZEND_HASH_UPDATE(&EG(persistent_list), hash_key, hash_key_len + 1, &resource, sizeof(php5to7_zend_resource_le));
                PHP_DRIVER_G(persistent_prepared_statements)++;
          #else
                resource.type = php_le_php_driver_prepared_statement();
                resource.ptr = pprepared_statement;
                PHP5TO7_ZEND_HASH_UPDATE(&EG(persistent_list), hash_key, hash_key_len + 1, resource, sizeof(php5to7_zend_resource_le));
                PHP_DRIVER_G(persistent_prepared_statements)++;
          #endif
      }
    }
  }

  if (self->persist) {
    if (php_driver_future_is_error(future TSRMLS_CC) == FAILURE) {
      (void) PHP5TO7_ZEND_HASH_DEL(&EG(persistent_list), hash_key, hash_key_len + 1);
    }
    efree(hash_key);
  }
  else {
    cass_future_free(future);
  }

}

PHP_METHOD(DefaultSession, prepareAsync)
{
  zval *cql = NULL;
  zval *options = NULL;
  php_driver_session *self = NULL;
  CassFuture *future = NULL;
  php_driver_future_prepared_statement *future_prepared = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &cql, &options) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  future = cass_session_prepare_n((CassSession *)self->session->data,
                                  Z_STRVAL_P(cql), Z_STRLEN_P(cql));

  object_init_ex(return_value, php_driver_future_prepared_statement_ce);
  future_prepared = PHP_DRIVER_GET_FUTURE_PREPARED_STATEMENT(return_value);

  future_prepared->future = future;
}

PHP_METHOD(DefaultSession, close)
{
  zval *timeout = NULL;
  CassFuture *future = NULL;
  php_driver_session *self;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());


  if (self->persist)
    return;

  future = cass_session_close((CassSession *) self->session->data);

  if (php_driver_future_wait_timed(future, timeout TSRMLS_CC) == SUCCESS)
    php_driver_future_is_error(future TSRMLS_CC);

  cass_future_free(future);
}

PHP_METHOD(DefaultSession, closeAsync)
{
  php_driver_session *self;
  php_driver_future_close *future = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_SESSION(getThis());

  if (self->persist) {
    object_init_ex(return_value, php_driver_future_value_ce);
    return;
  }

  object_init_ex(return_value, php_driver_future_close_ce);
  future = PHP_DRIVER_GET_FUTURE_CLOSE(return_value);

  future->future = cass_session_close((CassSession *) self->session->data);
}

PHP_METHOD(DefaultSession, metrics)
{
  CassMetrics metrics;
  php5to7_zval requests;
  php5to7_zval stats;
  php5to7_zval errors;
  php_driver_session *self = PHP_DRIVER_GET_SESSION(getThis());

  if (zend_parse_parameters_none() == FAILURE)
    return;

  cass_session_get_metrics((CassSession *)self->session->data, &metrics);

  PHP5TO7_ZVAL_MAYBE_MAKE(requests);
  array_init(PHP5TO7_ZVAL_MAYBE_P(requests));
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "min",
                 metrics.requests.min);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "max",
                 metrics.requests.max);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "mean",
                 metrics.requests.mean);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "stddev",
                 metrics.requests.stddev);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "median",
                 metrics.requests.median);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "p75",
                 metrics.requests.percentile_75th);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "p95",
                 metrics.requests.percentile_95th);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "p98",
                 metrics.requests.percentile_98th);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "p99",
                 metrics.requests.percentile_99th);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(requests),
                 "p999",
                 metrics.requests.percentile_999th);
  add_assoc_double(PHP5TO7_ZVAL_MAYBE_P(requests),
                   "mean_rate",
                   metrics.requests.mean_rate);
  add_assoc_double(PHP5TO7_ZVAL_MAYBE_P(requests),
                   "m1_rate",
                   metrics.requests.one_minute_rate);
  add_assoc_double(PHP5TO7_ZVAL_MAYBE_P(requests),
                   "m5_rate",
                   metrics.requests.five_minute_rate);
  add_assoc_double(PHP5TO7_ZVAL_MAYBE_P(requests),
                   "m15_rate",
                   metrics.requests.fifteen_minute_rate);

  PHP5TO7_ZVAL_MAYBE_MAKE(stats);
  array_init(PHP5TO7_ZVAL_MAYBE_P(stats));
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(stats),
                 "total_connections",
                 metrics.stats.total_connections);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(stats),
                "available_connections",
                metrics.stats.available_connections);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(stats),
                 "exceeded_pending_requests_water_mark",
                 metrics.stats.exceeded_pending_requests_water_mark);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(stats),
                 "exceeded_write_bytes_water_mark",
                 metrics.stats.exceeded_write_bytes_water_mark);

  PHP5TO7_ZVAL_MAYBE_MAKE(errors);
  array_init(PHP5TO7_ZVAL_MAYBE_P(errors));
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(errors),
                 "connection_timeouts",
                 metrics.errors.connection_timeouts);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(errors),
                 "pending_request_timeouts",
                 metrics.errors.pending_request_timeouts);
  add_assoc_long(PHP5TO7_ZVAL_MAYBE_P(errors),
                 "request_timeouts",
                 metrics.errors.request_timeouts);

  array_init(return_value);
  add_assoc_zval(return_value, "stats", PHP5TO7_ZVAL_MAYBE_P(stats));
  add_assoc_zval(return_value, "requests", PHP5TO7_ZVAL_MAYBE_P(requests));
  add_assoc_zval(return_value, "errors", PHP5TO7_ZVAL_MAYBE_P(errors));
}

PHP_METHOD(DefaultSession, schema)
{
  php_driver_session *self;
  php_driver_schema *schema;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = PHP_DRIVER_GET_SESSION(getThis());

  object_init_ex(return_value, php_driver_default_schema_ce);
  schema = PHP_DRIVER_GET_SCHEMA(return_value);

  schema->schema = php_driver_new_ref(
                     (void *) cass_session_get_schema_meta((CassSession *) self->session->data),
                     free_schema);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_execute, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, statement)
  ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_prepare, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, cql)
  ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_default_session_methods[] = {
  PHP_ME(DefaultSession, execute, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, executeAsync, arginfo_execute, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, prepare, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, prepareAsync, arginfo_prepare, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, close, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, closeAsync, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, metrics, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultSession, schema, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_default_session_handlers;

static HashTable *
php_driver_default_session_properties(php7to8_object *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_default_session_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_default_session_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_session *self = PHP5TO7_ZEND_OBJECT_GET(session, object);

  php_driver_del_peref(&self->session, 1);
  PHP5TO7_ZVAL_MAYBE_DESTROY(self->default_timeout);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_default_session_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_session *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(session, ce);

  self->session             = NULL;
  self->persist             = 0;
  self->default_consistency = PHP_DRIVER_DEFAULT_CONSISTENCY;
  self->default_page_size   = 5000;
  self->keyspace            = NULL;
  self->hash_key            = NULL;
  PHP5TO7_ZVAL_UNDEF(self->default_timeout);

  PHP5TO7_ZEND_OBJECT_INIT_EX(session, default_session, self, ce);
}

void php_driver_define_DefaultSession(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\DefaultSession", php_driver_default_session_methods);
  php_driver_default_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_default_session_ce TSRMLS_CC, 1, php_driver_session_ce);
  php_driver_default_session_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_default_session_ce->create_object = php_driver_default_session_new;

  memcpy(&php_driver_default_session_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_default_session_handlers.get_properties  = php_driver_default_session_properties;
  PHP7TO8_COMPARE(php_driver_default_session_handlers, php_driver_default_session_compare);
  php_driver_default_session_handlers.clone_obj = NULL;
}
