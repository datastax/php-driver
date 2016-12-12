/**
 * Copyright 2015-2016 DataStax, Inc.
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

#ifndef PHP_DRIVER_TYPES_H
#define PHP_DRIVER_TYPES_H

#if PHP_MAJOR_VERSION >= 7
#define PHP_DRIVER_BEGIN_OBJECT_TYPE(type_name) \
  typedef struct php_driver_##type_name##_ {

#define PHP_DRIVER_END_OBJECT_TYPE(type_name) \
    zend_object zval;                            \
  } php_driver_##type_name;                       \
  static inline php_driver_##type_name *php_driver_##type_name##_object_fetch(zend_object *obj) { \
    return (php_driver_##type_name *)((char *)obj - XtOffsetOf(php_driver_##type_name, zval));        \
  }
#else
#define PHP_DRIVER_BEGIN_OBJECT_TYPE(type_name) \
  typedef struct php_driver_##type_name##_ {        \
    zend_object zval;

#define PHP_DRIVER_END_OBJECT_TYPE(type_name) \
  } php_driver_##type_name;
#endif

#if PHP_MAJOR_VERSION >= 7
  #define PHP_DRIVER_GET_NUMERIC(obj) php_driver_numeric_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_BLOB(obj) php_driver_blob_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_TIMESTAMP(obj) php_driver_timestamp_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_DATE(obj) php_driver_date_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_TIME(obj) php_driver_time_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_UUID(obj) php_driver_uuid_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_INET(obj) php_driver_inet_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_COLLECTION(obj) php_driver_collection_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_MAP(obj) php_driver_map_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_SET(obj) php_driver_set_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_TUPLE(obj) php_driver_tuple_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_USER_TYPE_VALUE(obj) php_driver_user_type_value_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_CLUSTER(obj) php_driver_cluster_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_STATEMENT(obj) php_driver_statement_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_EXECUTION_OPTIONS(obj) php_driver_execution_options_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_ROWS(obj) php_driver_rows_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_FUTURE_ROWS(obj) php_driver_future_rows_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_CLUSTER_BUILDER(obj) php_driver_cluster_builder_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_FUTURE_PREPARED_STATEMENT(obj) php_driver_future_prepared_statement_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_FUTURE_VALUE(obj) php_driver_future_value_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_FUTURE_CLOSE(obj) php_driver_future_close_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_FUTURE_SESSION(obj) php_driver_future_session_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_SESSION(obj) php_driver_session_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_SSL(obj) php_driver_ssl_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_SSL_BUILDER(obj) php_driver_ssl_builder_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_SCHEMA(obj) php_driver_schema_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_KEYSPACE(obj) php_driver_keyspace_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_TABLE(obj) php_driver_table_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_COLUMN(obj) php_driver_column_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_INDEX(obj) php_driver_index_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_MATERIALIZED_VIEW(obj) php_driver_materialized_view_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_FUNCTION(obj) php_driver_function_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_AGGREGATE(obj) php_driver_aggregate_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_TYPE(obj) php_driver_type_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_RETRY_POLICY(obj) php_driver_retry_policy_object_fetch(Z_OBJ_P(obj))
  #define PHP_DRIVER_GET_TIMESTAMP_GEN(obj) php_driver_timestamp_gen_object_fetch(Z_OBJ_P(obj))
#else
  #define PHP_DRIVER_GET_NUMERIC(obj) ((php_driver_numeric *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_BLOB(obj) ((php_driver_blob *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_TIMESTAMP(obj) ((php_driver_timestamp *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_DATE(obj) ((php_driver_date *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_TIME(obj) ((php_driver_time *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_UUID(obj) ((php_driver_uuid *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_INET(obj) ((php_driver_inet *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_COLLECTION(obj) ((php_driver_collection *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_MAP(obj) ((php_driver_map *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_SET(obj) ((php_driver_set *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_TUPLE(obj) ((php_driver_tuple *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_USER_TYPE_VALUE(obj) ((php_driver_user_type_value *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_CLUSTER(obj) ((php_driver_cluster *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_STATEMENT(obj) ((php_driver_statement *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_EXECUTION_OPTIONS(obj) ((php_driver_execution_options *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_ROWS(obj) ((php_driver_rows *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_FUTURE_ROWS(obj) ((php_driver_future_rows *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_CLUSTER_BUILDER(obj) ((php_driver_cluster_builder *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_FUTURE_PREPARED_STATEMENT(obj) ((php_driver_future_prepared_statement *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_FUTURE_VALUE(obj) ((php_driver_future_value *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_FUTURE_CLOSE(obj) ((php_driver_future_close *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_FUTURE_SESSION(obj) ((php_driver_future_session *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_SESSION(obj) ((php_driver_session *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_SSL(obj) ((php_driver_ssl *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_SSL_BUILDER(obj) ((php_driver_ssl_builder *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_SCHEMA(obj) ((php_driver_schema *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_KEYSPACE(obj) ((php_driver_keyspace *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_TABLE(obj) ((php_driver_table *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_COLUMN(obj) ((php_driver_column *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_INDEX(obj) ((php_driver_index *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_MATERIALIZED_VIEW(obj) ((php_driver_materialized_view *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_FUNCTION(obj) ((php_driver_function *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_AGGREGATE(obj) ((php_driver_aggregate *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_TYPE(obj) ((php_driver_type *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_RETRY_POLICY(obj) ((php_driver_retry_policy *)zend_object_store_get_object((obj) TSRMLS_CC))
  #define PHP_DRIVER_GET_TIMESTAMP_GEN(obj) ((php_driver_timestamp_gen *)zend_object_store_get_object((obj) TSRMLS_CC))
#endif

typedef enum {
  PHP_DRIVER_BIGINT,
  PHP_DRIVER_DECIMAL,
  PHP_DRIVER_FLOAT,
  PHP_DRIVER_VARINT,
  PHP_DRIVER_SMALLINT,
  PHP_DRIVER_TINYINT
} php_driver_numeric_type;

PHP_DRIVER_BEGIN_OBJECT_TYPE(numeric)
  php_driver_numeric_type type;
  union {
    cass_int8_t tinyint_value;
    cass_int16_t smallint_value;
    cass_int64_t bigint_value;
    cass_float_t float_value;
    mpz_t varint_value;
    struct {
      mpz_t decimal_value;
      long decimal_scale;
    };
  };
PHP_DRIVER_END_OBJECT_TYPE(numeric)

PHP_DRIVER_BEGIN_OBJECT_TYPE(timestamp)
    cass_int64_t timestamp;
PHP_DRIVER_END_OBJECT_TYPE(timestamp)

PHP_DRIVER_BEGIN_OBJECT_TYPE(date)
    cass_uint32_t date;
PHP_DRIVER_END_OBJECT_TYPE(date)

PHP_DRIVER_BEGIN_OBJECT_TYPE(time)
    cass_int64_t time;
PHP_DRIVER_END_OBJECT_TYPE(time)

PHP_DRIVER_BEGIN_OBJECT_TYPE(blob)
  cass_byte_t *data;
  size_t size;
PHP_DRIVER_END_OBJECT_TYPE(blob)

PHP_DRIVER_BEGIN_OBJECT_TYPE(uuid)
  CassUuid uuid;
PHP_DRIVER_END_OBJECT_TYPE(uuid)

PHP_DRIVER_BEGIN_OBJECT_TYPE(inet)
  CassInet inet;
PHP_DRIVER_END_OBJECT_TYPE(inet)

PHP_DRIVER_BEGIN_OBJECT_TYPE(collection)
  php5to7_zval type;
  HashTable values;
  unsigned hashv;
  int dirty;
PHP_DRIVER_END_OBJECT_TYPE(collection)

typedef struct php_driver_map_entry_ php_driver_map_entry;

PHP_DRIVER_BEGIN_OBJECT_TYPE(map)
  php5to7_zval type;
  php_driver_map_entry *entries;
  unsigned hashv;
  int dirty;
  php_driver_map_entry *iter_curr;
  php_driver_map_entry *iter_temp;
PHP_DRIVER_END_OBJECT_TYPE(map)

typedef struct php_driver_set_entry_ php_driver_set_entry;

PHP_DRIVER_BEGIN_OBJECT_TYPE(set)
  php5to7_zval type;
  php_driver_set_entry *entries;
  unsigned hashv;
  int dirty;
  php_driver_set_entry *iter_curr;
  php_driver_set_entry *iter_temp;
  int iter_index;
PHP_DRIVER_END_OBJECT_TYPE(set)

PHP_DRIVER_BEGIN_OBJECT_TYPE(tuple)
  php5to7_zval type;
  HashTable values;
  HashPosition pos;
  unsigned hashv;
  int dirty;
PHP_DRIVER_END_OBJECT_TYPE(tuple)

PHP_DRIVER_BEGIN_OBJECT_TYPE(user_type_value)
  php5to7_zval type;
  HashTable values;
  HashPosition pos;
  unsigned hashv;
  int dirty;
PHP_DRIVER_END_OBJECT_TYPE(user_type_value)

PHP_DRIVER_BEGIN_OBJECT_TYPE(cluster)
  cass_byte_t *data;
  CassCluster *cluster;
  long default_consistency;
  int default_page_size;
  php5to7_zval default_timeout;
  cass_bool_t persist;
  char *hash_key;
  int hash_key_len;
PHP_DRIVER_END_OBJECT_TYPE(cluster)

typedef enum {
  PHP_DRIVER_SIMPLE_STATEMENT,
  PHP_DRIVER_PREPARED_STATEMENT,
  PHP_DRIVER_BATCH_STATEMENT
} php_driver_statement_type;

PHP_DRIVER_BEGIN_OBJECT_TYPE(statement)
  php_driver_statement_type type;
  union {
    char *cql;
    const CassPrepared *prepared;
    struct {
      CassBatchType batch_type;
      HashTable statements;
    };
  };
PHP_DRIVER_END_OBJECT_TYPE(statement)

typedef struct {
  php5to7_zval statement;
  php5to7_zval arguments;
} php_driver_batch_statement_entry;

PHP_DRIVER_BEGIN_OBJECT_TYPE(execution_options)
  long consistency;
  long serial_consistency;
  int page_size;
  char *paging_state_token;
  size_t paging_state_token_size;
  php5to7_zval timeout;
  php5to7_zval arguments;
  php5to7_zval retry_policy;
  cass_int64_t timestamp;
PHP_DRIVER_END_OBJECT_TYPE(execution_options)

typedef enum {
  LOAD_BALANCING_DEFAULT = 0,
  LOAD_BALANCING_ROUND_ROBIN,
  LOAD_BALANCING_DC_AWARE_ROUND_ROBIN
} php_driver_load_balancing;

typedef void (*php_driver_free_function)(void *data);

typedef struct {
  size_t                  count;
  php_driver_free_function destruct;
  void                   *data;
} php_driver_ref;

PHP_DRIVER_BEGIN_OBJECT_TYPE(rows)
  php_driver_ref *statement;
  php_driver_ref *session;
  php5to7_zval rows;
  php5to7_zval next_rows;
  php_driver_ref *result;
  php_driver_ref *next_result;
  php5to7_zval future_next_page;
PHP_DRIVER_END_OBJECT_TYPE(rows)

PHP_DRIVER_BEGIN_OBJECT_TYPE(future_rows)
  php_driver_ref *statement;
  php_driver_ref *session;
  php5to7_zval rows;
  php_driver_ref *result;
  CassFuture *future;
PHP_DRIVER_END_OBJECT_TYPE(future_rows)

PHP_DRIVER_BEGIN_OBJECT_TYPE(cluster_builder)
  char *contact_points;
  int port;
  php_driver_load_balancing load_balancing_policy;
  char *local_dc;
  unsigned int used_hosts_per_remote_dc;
  cass_bool_t allow_remote_dcs_for_local_cl;
  cass_bool_t use_token_aware_routing;
  char *username;
  char *password;
  unsigned int connect_timeout;
  unsigned int request_timeout;
  php5to7_zval ssl_options;
  long default_consistency;
  int default_page_size;
  php5to7_zval default_timeout;
  cass_bool_t persist;
  int protocol_version;
  int io_threads;
  int core_connections_per_host;
  int max_connections_per_host;
  unsigned int reconnect_interval;
  cass_bool_t enable_latency_aware_routing;
  cass_bool_t enable_tcp_nodelay;
  cass_bool_t enable_tcp_keepalive;
  unsigned int tcp_keepalive_delay;
  php5to7_zval retry_policy;
  php5to7_zval timestamp_gen;
  cass_bool_t enable_schema;
  char *blacklist_hosts;
  char *whitelist_hosts;
  char *blacklist_dcs;
  char *whitelist_dcs;
  cass_bool_t enable_hostname_resolution;
  cass_bool_t enable_randomized_contact_points;
  unsigned int connection_heartbeat_interval;
PHP_DRIVER_END_OBJECT_TYPE(cluster_builder)

PHP_DRIVER_BEGIN_OBJECT_TYPE(future_prepared_statement)
  CassFuture *future;
  php5to7_zval prepared_statement;
PHP_DRIVER_END_OBJECT_TYPE(future_prepared_statement)

PHP_DRIVER_BEGIN_OBJECT_TYPE(future_value)
  php5to7_zval value;
PHP_DRIVER_END_OBJECT_TYPE(future_value)

PHP_DRIVER_BEGIN_OBJECT_TYPE(future_close)
  CassFuture *future;
PHP_DRIVER_END_OBJECT_TYPE(future_close)

PHP_DRIVER_BEGIN_OBJECT_TYPE(future_session)
  CassFuture *future;
  php_driver_ref *session;
  php5to7_zval default_session;
  cass_bool_t persist;
  char *hash_key;
  int hash_key_len;
  char *exception_message;
  CassError exception_code;
PHP_DRIVER_END_OBJECT_TYPE(future_session)

typedef struct {
  CassFuture *future;
  php_driver_ref *session;
} php_driver_psession;

PHP_DRIVER_BEGIN_OBJECT_TYPE(session)
  php_driver_ref *session;
  long default_consistency;
  int default_page_size;
  php5to7_zval default_timeout;
  cass_bool_t persist;
PHP_DRIVER_END_OBJECT_TYPE(session)

PHP_DRIVER_BEGIN_OBJECT_TYPE(ssl)
  CassSsl *ssl;
PHP_DRIVER_END_OBJECT_TYPE(ssl)

PHP_DRIVER_BEGIN_OBJECT_TYPE(ssl_builder)
  int flags;
  char **trusted_certs;
  int trusted_certs_cnt;
  char *client_cert;
  char *private_key;
  char *passphrase;
PHP_DRIVER_END_OBJECT_TYPE(ssl_builder)

PHP_DRIVER_BEGIN_OBJECT_TYPE(schema)
  php_driver_ref *schema;
PHP_DRIVER_END_OBJECT_TYPE(schema)

PHP_DRIVER_BEGIN_OBJECT_TYPE(keyspace)
  php_driver_ref *schema;
  const CassKeyspaceMeta *meta;
PHP_DRIVER_END_OBJECT_TYPE(keyspace)

PHP_DRIVER_BEGIN_OBJECT_TYPE(table)
  php5to7_zval name;
  php5to7_zval options;
  php5to7_zval partition_key;
  php5to7_zval primary_key;
  php5to7_zval clustering_key;
  php5to7_zval clustering_order;
  php_driver_ref *schema;
  const CassTableMeta *meta;
PHP_DRIVER_END_OBJECT_TYPE(table)

PHP_DRIVER_BEGIN_OBJECT_TYPE(materialized_view)
  php5to7_zval name;
  php5to7_zval options;
  php5to7_zval partition_key;
  php5to7_zval primary_key;
  php5to7_zval clustering_key;
  php5to7_zval clustering_order;
  php5to7_zval base_table;
  php_driver_ref *schema;
  const CassMaterializedViewMeta *meta;
PHP_DRIVER_END_OBJECT_TYPE(materialized_view)

PHP_DRIVER_BEGIN_OBJECT_TYPE(column)
  php5to7_zval name;
  php5to7_zval type;
  int reversed;
  int frozen;
  php_driver_ref *schema;
  const CassColumnMeta *meta;
PHP_DRIVER_END_OBJECT_TYPE(column)

PHP_DRIVER_BEGIN_OBJECT_TYPE(index)
  php5to7_zval name;
  php5to7_zval kind;
  php5to7_zval target;
  php5to7_zval options;
  php_driver_ref *schema;
  const CassIndexMeta *meta;
PHP_DRIVER_END_OBJECT_TYPE(index)

PHP_DRIVER_BEGIN_OBJECT_TYPE(function)
  php5to7_zval simple_name;
  php5to7_zval arguments;
  php5to7_zval return_type;
  php5to7_zval signature;
  php5to7_zval language;
  php5to7_zval body;
  php_driver_ref *schema;
  const CassFunctionMeta *meta;
PHP_DRIVER_END_OBJECT_TYPE(function)

PHP_DRIVER_BEGIN_OBJECT_TYPE(aggregate)
  php5to7_zval simple_name;
  php5to7_zval argument_types;
  php5to7_zval state_function;
  php5to7_zval final_function;
  php5to7_zval initial_condition;
  php5to7_zval state_type;
  php5to7_zval return_type;
  php5to7_zval signature;
  php_driver_ref *schema;
  const CassAggregateMeta *meta;
PHP_DRIVER_END_OBJECT_TYPE(aggregate)

PHP_DRIVER_BEGIN_OBJECT_TYPE(type)
  CassValueType type;
  CassDataType *data_type;
  union {
    struct {
      php5to7_zval key_type;
      php5to7_zval value_type;
    };
    char *name;
    struct {
      char *keyspace;
      char *type_name;
      HashTable types;
    };
  };
PHP_DRIVER_END_OBJECT_TYPE(type)

PHP_DRIVER_BEGIN_OBJECT_TYPE(retry_policy)
  CassRetryPolicy *policy;
PHP_DRIVER_END_OBJECT_TYPE(retry_policy)

PHP_DRIVER_BEGIN_OBJECT_TYPE(timestamp_gen)
  CassTimestampGen *gen;
PHP_DRIVER_END_OBJECT_TYPE(timestamp_gen)

typedef unsigned (*php_driver_value_hash_t)(zval *obj TSRMLS_DC);

typedef struct {
  zend_object_handlers std;
  php_driver_value_hash_t hash_value;
} php_driver_value_handlers;

extern PHP_DRIVER_API zend_class_entry *php_driver_value_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_numeric_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_bigint_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_smallint_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_tinyint_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_blob_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_decimal_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_float_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_inet_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_timestamp_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_date_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_time_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_uuid_interface_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_uuid_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_timeuuid_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_varint_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_custom_ce;

extern PHP_DRIVER_API zend_class_entry *php_driver_set_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_map_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_collection_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_tuple_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_user_type_value_ce;

/* Exceptions */
void php_driver_define_Exception(TSRMLS_D);
void php_driver_define_InvalidArgumentException(TSRMLS_D);
void php_driver_define_DomainException(TSRMLS_D);
void php_driver_define_LogicException(TSRMLS_D);
void php_driver_define_RuntimeException(TSRMLS_D);
void php_driver_define_TimeoutException(TSRMLS_D);
void php_driver_define_ExecutionException(TSRMLS_D);
void php_driver_define_ReadTimeoutException(TSRMLS_D);
void php_driver_define_WriteTimeoutException(TSRMLS_D);
void php_driver_define_UnavailableException(TSRMLS_D);
void php_driver_define_TruncateException(TSRMLS_D);
void php_driver_define_ValidationException(TSRMLS_D);
void php_driver_define_InvalidQueryException(TSRMLS_D);
void php_driver_define_InvalidSyntaxException(TSRMLS_D);
void php_driver_define_UnauthorizedException(TSRMLS_D);
void php_driver_define_UnpreparedException(TSRMLS_D);
void php_driver_define_ConfigurationException(TSRMLS_D);
void php_driver_define_AlreadyExistsException(TSRMLS_D);
void php_driver_define_AuthenticationException(TSRMLS_D);
void php_driver_define_ProtocolException(TSRMLS_D);
void php_driver_define_ServerException(TSRMLS_D);
void php_driver_define_IsBootstrappingException(TSRMLS_D);
void php_driver_define_OverloadedException(TSRMLS_D);
void php_driver_define_DivideByZeroException(TSRMLS_D);
void php_driver_define_RangeException(TSRMLS_D);

/* Types */
void php_driver_define_Value(TSRMLS_D);
void php_driver_define_Numeric(TSRMLS_D);
void php_driver_define_Bigint(TSRMLS_D);
void php_driver_define_Smallint(TSRMLS_D);
void php_driver_define_Tinyint(TSRMLS_D);
void php_driver_define_Blob(TSRMLS_D);
void php_driver_define_Collection(TSRMLS_D);
void php_driver_define_Decimal(TSRMLS_D);
void php_driver_define_Float(TSRMLS_D);
void php_driver_define_Inet(TSRMLS_D);
void php_driver_define_Map(TSRMLS_D);
void php_driver_define_Set(TSRMLS_D);
void php_driver_define_Timestamp(TSRMLS_D);
void php_driver_define_Date(TSRMLS_D);
void php_driver_define_Time(TSRMLS_D);
void php_driver_define_Tuple(TSRMLS_D);
void php_driver_define_UserTypeValue(TSRMLS_D);
void php_driver_define_UuidInterface(TSRMLS_D);
void php_driver_define_Uuid(TSRMLS_D);
void php_driver_define_Timeuuid(TSRMLS_D);
void php_driver_define_Varint(TSRMLS_D);
void php_driver_define_Custom(TSRMLS_D);

/* Classes */
extern PHP_DRIVER_API zend_class_entry *php_driver_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_cluster_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_cluster_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_cluster_builder_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_cluster_builder_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_ssl_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_ssl_builder_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_future_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_future_prepared_statement_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_future_rows_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_future_session_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_future_value_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_future_close_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_session_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_session_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_runtime_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_timeout_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_logic_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_domain_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_invalid_argument_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_server_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_overloaded_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_is_bootstrapping_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_execution_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_truncate_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_write_timeout_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_read_timeout_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_truncate_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_unavailable_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_validation_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_invalid_syntax_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_unauthorized_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_invalid_query_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_configuration_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_already_exists_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_unprepared_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_protocol_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_authentication_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_divide_by_zero_exception_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_range_exception_ce;

extern PHP_DRIVER_API zend_class_entry *php_driver_statement_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_simple_statement_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_prepared_statement_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_batch_statement_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_execution_options_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_rows_ce;

void php_driver_define_Core(TSRMLS_D);
void php_driver_define_Cluster(TSRMLS_D);
void php_driver_define_ClusterBuilder(TSRMLS_D);
void php_driver_define_DefaultCluster(TSRMLS_D);
void php_driver_define_Future(TSRMLS_D);
void php_driver_define_FuturePreparedStatement(TSRMLS_D);
void php_driver_define_FutureRows(TSRMLS_D);
void php_driver_define_FutureSession(TSRMLS_D);
void php_driver_define_FutureValue(TSRMLS_D);
void php_driver_define_FutureClose(TSRMLS_D);
void php_driver_define_Session(TSRMLS_D);
void php_driver_define_DefaultSession(TSRMLS_D);
void php_driver_define_SSLOptions(TSRMLS_D);
void php_driver_define_SSLOptionsBuilder(TSRMLS_D);
void php_driver_define_Statement(TSRMLS_D);
void php_driver_define_SimpleStatement(TSRMLS_D);
void php_driver_define_PreparedStatement(TSRMLS_D);
void php_driver_define_BatchStatement(TSRMLS_D);
void php_driver_define_ExecutionOptions(TSRMLS_D);
void php_driver_define_Rows(TSRMLS_D);

extern PHP_DRIVER_API zend_class_entry *php_driver_schema_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_schema_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_keyspace_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_keyspace_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_table_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_table_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_column_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_column_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_index_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_index_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_materialized_view_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_materialized_view_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_function_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_function_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_aggregate_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_default_aggregate_ce;

void php_driver_define_Schema(TSRMLS_D);
void php_driver_define_DefaultSchema(TSRMLS_D);
void php_driver_define_Keyspace(TSRMLS_D);
void php_driver_define_DefaultKeyspace(TSRMLS_D);
void php_driver_define_Table(TSRMLS_D);
void php_driver_define_DefaultTable(TSRMLS_D);
void php_driver_define_Column(TSRMLS_D);
void php_driver_define_DefaultColumn(TSRMLS_D);
void php_driver_define_Index(TSRMLS_D);
void php_driver_define_DefaultIndex(TSRMLS_D);
void php_driver_define_MaterializedView(TSRMLS_D);
void php_driver_define_DefaultMaterializedView(TSRMLS_D);
void php_driver_define_Function(TSRMLS_D);
void php_driver_define_DefaultFunction(TSRMLS_D);
void php_driver_define_Aggregate(TSRMLS_D);
void php_driver_define_DefaultAggregate(TSRMLS_D);

extern PHP_DRIVER_API zend_class_entry *php_driver_type_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_type_scalar_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_type_collection_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_type_set_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_type_map_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_type_tuple_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_type_user_type_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_type_custom_ce;

void php_driver_define_Type(TSRMLS_D);
void php_driver_define_TypeScalar(TSRMLS_D);
void php_driver_define_TypeCollection(TSRMLS_D);
void php_driver_define_TypeSet(TSRMLS_D);
void php_driver_define_TypeMap(TSRMLS_D);
void php_driver_define_TypeTuple(TSRMLS_D);
void php_driver_define_TypeUserType(TSRMLS_D);
void php_driver_define_TypeCustom(TSRMLS_D);

extern PHP_DRIVER_API zend_class_entry *php_driver_retry_policy_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_retry_policy_default_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_retry_policy_downgrading_consistency_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_retry_policy_fallthrough_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_retry_policy_logging_ce;

void php_driver_define_RetryPolicy(TSRMLS_D);
void php_driver_define_RetryPolicyDefault(TSRMLS_D);
void php_driver_define_RetryPolicyDowngradingConsistency(TSRMLS_D);
void php_driver_define_RetryPolicyFallthrough(TSRMLS_D);
void php_driver_define_RetryPolicyLogging(TSRMLS_D);

extern PHP_DRIVER_API zend_class_entry *php_driver_timestamp_gen_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_timestamp_gen_monotonic_ce;
extern PHP_DRIVER_API zend_class_entry *php_driver_timestamp_gen_server_side_ce;

void php_driver_define_TimestampGenerator(TSRMLS_D);
void php_driver_define_TimestampGeneratorMonotonic(TSRMLS_D);
void php_driver_define_TimestampGeneratorServerSide(TSRMLS_D);

extern int php_le_php_driver_cluster();
extern int php_le_php_driver_session();

#endif /* PHP_DRIVER_TYPES_H */
