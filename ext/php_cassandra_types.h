#ifndef PHP_CASSANDRA_TYPES_H
#define PHP_CASSANDRA_TYPES_H

#if PHP_MAJOR_VERSION >= 7
#define PHP_CASSANDRA_BEGIN_OBJECT_TYPE(type_name) \
  typedef struct cassandra_##type_name##_ {

#define PHP_CASSANDRA_END_OBJECT_TYPE(type_name) \
    zend_object zval;                            \
  } cassandra_##type_name;                       \
  static inline cassandra_##type_name* php_cassandra_##type_name##_object_fetch(zend_object* obj) { \
    return (cassandra_##type_name *)((char *)obj - XtOffsetOf(cassandra_##type_name, zval));        \
  }
#else
#define PHP_CASSANDRA_BEGIN_OBJECT_TYPE(type_name) \
  typedef struct cassandra_##type_name##_ {        \
    zend_object zval;

#define PHP_CASSANDRA_END_OBJECT_TYPE(type_name) \
  } cassandra_##type_name;
#endif

#if PHP_MAJOR_VERSION >= 7
  #define PHP_CASSANDRA_GET_NUMERIC(obj) php_cassandra_numeric_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_BLOB(obj) php_cassandra_blob_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_TIMESTAMP(obj) php_cassandra_timestamp_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_UUID(obj) php_cassandra_uuid_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_INET(obj) php_cassandra_inet_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_COLLECTION(obj) php_cassandra_collection_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_MAP(obj) php_cassandra_map_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_SET(obj) php_cassandra_set_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_CLUSTER(obj) php_cassandra_cluster_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_STATEMENT(obj) php_cassandra_statement_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_EXECUTION_OPTIONS(obj) php_cassandra_execution_options_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_ROWS(obj) php_cassandra_rows_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_FUTURE_ROWS(obj) php_cassandra_future_rows_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_CLUSTER_BUILDER(obj) php_cassandra_cluster_builder_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_FUTURE_PREPARED_STATEMENT(obj) php_cassandra_future_prepared_statement_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_FUTURE_VALUE(obj) php_cassandra_future_value_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_FUTURE_CLOSE(obj) php_cassandra_future_close_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_FUTURE_SESSION(obj) php_cassandra_future_session_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_SESSION(obj) php_cassandra_session_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_SSL(obj) php_cassandra_ssl_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_SSL_BUILDER(obj) php_cassandra_ssl_builder_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_SCHEMA(obj) php_cassandra_schema_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_KEYSPACE(obj) php_cassandra_keyspace_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_TABLE(obj) php_cassandra_table_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_COLUMN(obj) php_cassandra_column_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_TYPE_SCALAR(obj) php_cassandra_type_scalar_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_TYPE_COLLECTION(obj) php_cassandra_type_collection_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_TYPE_MAP(obj) php_cassandra_type_map_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_TYPE_SET(obj) php_cassandra_type_set_object_fetch(Z_OBJ_P(obj))
  #define PHP_CASSANDRA_GET_TYPE_CUSTOM(obj) php_cassandra_type_custom_object_fetch(Z_OBJ_P(obj))
#else
  #define PHP_CASSANDRA_GET_NUMERIC(obj) (cassandra_numeric *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_BLOB(obj) (cassandra_blob *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_TIMESTAMP(obj) (cassandra_timestamp *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_UUID(obj) (cassandra_uuid *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_INET(obj) (cassandra_inet *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_COLLECTION(obj) (cassandra_collection *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_MAP(obj) (cassandra_map *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_SET(obj) (cassandra_set *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_CLUSTER(obj) (cassandra_cluster *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_STATEMENT(obj) (cassandra_statement *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_EXECUTION_OPTIONS(obj) (cassandra_execution_options *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_ROWS(obj) (cassandra_rows *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_FUTURE_ROWS(obj) (cassandra_future_rows *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_CLUSTER_BUILDER(obj) (cassandra_cluster_builder *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_FUTURE_PREPARED_STATEMENT(obj) (cassandra_future_prepared_statement *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_FUTURE_VALUE(obj) (cassandra_future_value *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_FUTURE_CLOSE(obj) (cassandra_future_close *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_FUTURE_SESSION(obj) (cassandra_future_session *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_SESSION(obj) (cassandra_session *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_SSL(obj) (cassandra_ssl *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_SSL_BUILDER(obj) (cassandra_ssl_builder *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_SCHEMA(obj) (cassandra_schema *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_KEYSPACE(obj) (cassandra_keyspace *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_TABLE(obj) (cassandra_table *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_COLUMN(obj) (cassandra_column *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_TYPE_SCALAR(obj) (cassandra_type_scalar *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_TYPE_COLLECTION(obj) (cassandra_type_collection *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_TYPE_MAP(obj) (cassandra_type_map *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_TYPE_SET(obj) (cassandra_type_set *)zend_object_store_get_object((obj) TSRMLS_CC)
  #define PHP_CASSANDRA_GET_TYPE_CUSTOM(obj) (cassandra_type_custom *)zend_object_store_get_object((obj) TSRMLS_CC)
#endif

typedef enum {
  CASSANDRA_BIGINT,
  CASSANDRA_DECIMAL,
  CASSANDRA_FLOAT,
  CASSANDRA_VARINT
} cassandra_numeric_type;

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(numeric)
  cassandra_numeric_type type;
  union {
    cass_int64_t bigint_value;
    cass_float_t float_value;
    mpz_t varint_value;
    struct {
      mpz_t decimal_value;
      long decimal_scale;
    };
  };
PHP_CASSANDRA_END_OBJECT_TYPE(numeric)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(timestamp)
    cass_int64_t timestamp;
PHP_CASSANDRA_END_OBJECT_TYPE(timestamp)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(blob)
  cass_byte_t* data;
  size_t size;
PHP_CASSANDRA_END_OBJECT_TYPE(blob)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(uuid)
  CassUuid uuid;
PHP_CASSANDRA_END_OBJECT_TYPE(uuid)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(inet)
  CassInet inet;
PHP_CASSANDRA_END_OBJECT_TYPE(inet)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(collection)
  cass_byte_t* data;
  CassValueType type;
  HashTable values;
PHP_CASSANDRA_END_OBJECT_TYPE(collection)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(map)
  CassValueType key_type;
  HashTable keys;
  CassValueType value_type;
  HashTable values;
PHP_CASSANDRA_END_OBJECT_TYPE(map)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(set)
  CassValueType type;
  HashTable values;
  int pos;
PHP_CASSANDRA_END_OBJECT_TYPE(set)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(cluster)
  cass_byte_t* data;
  CassCluster* cluster;
  long default_consistency;
  int default_page_size;
  php5to7_zval default_timeout;
  cass_bool_t persist;
  char* hash_key;
  int hash_key_len;
PHP_CASSANDRA_END_OBJECT_TYPE(cluster)

typedef enum {
  CASSANDRA_SIMPLE_STATEMENT,
  CASSANDRA_PREPARED_STATEMENT,
  CASSANDRA_BATCH_STATEMENT
} cassandra_statement_type;

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(statement)
  cassandra_statement_type type;
  union {
    char* cql;
    const CassPrepared* prepared;
    struct {
      CassBatchType batch_type;
      HashTable statements;
    };
  };
PHP_CASSANDRA_END_OBJECT_TYPE(statement)

typedef struct {
  php5to7_zval statement;
  php5to7_zval arguments;
} cassandra_batch_statement_entry;

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(execution_options)
  long consistency;
  long serial_consistency;
  int page_size;
  php5to7_zval timeout;
  php5to7_zval arguments;
PHP_CASSANDRA_END_OBJECT_TYPE(execution_options)

typedef enum {
  LOAD_BALANCING_ROUND_ROBIN = 0,
  LOAD_BALANCING_DC_AWARE_ROUND_ROBIN
} cassandra_load_balancing;

typedef void (*cassandra_free_function)(void* data);

typedef struct {
  size_t                  count;
  cassandra_free_function destruct;
  void*                   data;
} cassandra_ref;

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(rows)
  cassandra_ref* statement;
  php5to7_zval session;
  php5to7_zval rows;
  const CassResult* result;
  php5to7_zval next_page;
  php5to7_zval future_next_page;
PHP_CASSANDRA_END_OBJECT_TYPE(rows)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(future_rows)
  cassandra_ref* statement;
  php5to7_zval session;
  php5to7_zval rows;
  CassFuture* future;
PHP_CASSANDRA_END_OBJECT_TYPE(future_rows)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(cluster_builder)
  char* contact_points;
  int port;
  cassandra_load_balancing load_balancing_policy;
  char* local_dc;
  unsigned int used_hosts_per_remote_dc;
  cass_bool_t allow_remote_dcs_for_local_cl;
  cass_bool_t use_token_aware_routing;
  char* username;
  char* password;
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
PHP_CASSANDRA_END_OBJECT_TYPE(cluster_builder)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(future_prepared_statement)
  CassFuture* future;
  php5to7_zval prepared_statement;
PHP_CASSANDRA_END_OBJECT_TYPE(future_prepared_statement)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(future_value)
  php5to7_zval value;
PHP_CASSANDRA_END_OBJECT_TYPE(future_value)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(future_close)
  CassFuture* future;
PHP_CASSANDRA_END_OBJECT_TYPE(future_close)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(future_session)
  CassFuture* future;
  CassSession* session;
  php5to7_zval default_session;
  cass_bool_t persist;
  char* hash_key;
  int hash_key_len;
  char* exception_message;
  CassError exception_code;
PHP_CASSANDRA_END_OBJECT_TYPE(future_session)

typedef struct {
  CassFuture* future;
  CassSession* session;
} cassandra_psession;

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(session)
  CassSession* session;
  long default_consistency;
  int default_page_size;
  php5to7_zval default_timeout;
  cass_bool_t persist;
PHP_CASSANDRA_END_OBJECT_TYPE(session)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(ssl)
  CassSsl* ssl;
PHP_CASSANDRA_END_OBJECT_TYPE(ssl)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(ssl_builder)
  int flags;
  char** trusted_certs;
  int trusted_certs_cnt;
  char* client_cert;
  char* private_key;
  char* passphrase;
PHP_CASSANDRA_END_OBJECT_TYPE(ssl_builder)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(schema)
  cassandra_ref* schema;
PHP_CASSANDRA_END_OBJECT_TYPE(schema)

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 2, 0)
typedef const CassKeyspaceMeta cassandra_keyspace_meta;
#else
typedef const CassSchemaMeta cassandra_keyspace_meta;
#endif

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(keyspace)
  cassandra_ref* schema;
  cassandra_keyspace_meta* meta;
PHP_CASSANDRA_END_OBJECT_TYPE(keyspace)

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 2, 0)
typedef const CassTableMeta cassandra_table_meta;
#else
typedef const CassSchemaMeta cassandra_table_meta;
#endif

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(table)
  cassandra_ref* schema;
  cassandra_table_meta* meta;
PHP_CASSANDRA_END_OBJECT_TYPE(table)

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 2, 0)
typedef const CassColumnMeta cassandra_column_meta;
#else
typedef const CassSchemaMeta cassandra_column_meta;
#endif

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(column)
  php5to7_zval name;
  php5to7_zval type;
  int reversed;
  int frozen;
  cassandra_ref* schema;
  cassandra_column_meta* meta;
PHP_CASSANDRA_END_OBJECT_TYPE(column)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(type_scalar)
  CassValueType type;
PHP_CASSANDRA_END_OBJECT_TYPE(type_scalar)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(type_collection)
  CassValueType type;
PHP_CASSANDRA_END_OBJECT_TYPE(type_collection)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(type_map)
  CassValueType key_type;
  CassValueType value_type;
PHP_CASSANDRA_END_OBJECT_TYPE(type_map)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(type_set)
  CassValueType type;
PHP_CASSANDRA_END_OBJECT_TYPE(type_set)

PHP_CASSANDRA_BEGIN_OBJECT_TYPE(type_custom)
  char* name;
PHP_CASSANDRA_END_OBJECT_TYPE(type_custom)

extern PHP_CASSANDRA_API zend_class_entry* cassandra_numeric_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_bigint_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_blob_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_decimal_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_float_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_inet_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_timestamp_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_uuid_interface_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_uuid_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_timeuuid_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_varint_ce;

extern PHP_CASSANDRA_API zend_class_entry* cassandra_set_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_map_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_collection_ce;

/* Exceptions */
void cassandra_define_Exception(TSRMLS_D);
void cassandra_define_InvalidArgumentException(TSRMLS_D);
void cassandra_define_DomainException(TSRMLS_D);
void cassandra_define_LogicException(TSRMLS_D);
void cassandra_define_RuntimeException(TSRMLS_D);
void cassandra_define_TimeoutException(TSRMLS_D);
void cassandra_define_ExecutionException(TSRMLS_D);
void cassandra_define_ReadTimeoutException(TSRMLS_D);
void cassandra_define_WriteTimeoutException(TSRMLS_D);
void cassandra_define_UnavailableException(TSRMLS_D);
void cassandra_define_TruncateException(TSRMLS_D);
void cassandra_define_ValidationException(TSRMLS_D);
void cassandra_define_InvalidQueryException(TSRMLS_D);
void cassandra_define_InvalidSyntaxException(TSRMLS_D);
void cassandra_define_UnauthorizedException(TSRMLS_D);
void cassandra_define_UnpreparedException(TSRMLS_D);
void cassandra_define_ConfigurationException(TSRMLS_D);
void cassandra_define_AlreadyExistsException(TSRMLS_D);
void cassandra_define_AuthenticationException(TSRMLS_D);
void cassandra_define_ProtocolException(TSRMLS_D);
void cassandra_define_ServerException(TSRMLS_D);
void cassandra_define_IsBootstrappingException(TSRMLS_D);
void cassandra_define_OverloadedException(TSRMLS_D);
void cassandra_define_DivideByZeroException(TSRMLS_D);
void cassandra_define_RangeException(TSRMLS_D);

/* Types */
void cassandra_define_Numeric(TSRMLS_D);
void cassandra_define_Bigint(TSRMLS_D);
void cassandra_define_Blob(TSRMLS_D);
void cassandra_define_Collection(TSRMLS_D);
void cassandra_define_Decimal(TSRMLS_D);
void cassandra_define_Float(TSRMLS_D);
void cassandra_define_Inet(TSRMLS_D);
void cassandra_define_Map(TSRMLS_D);
void cassandra_define_Set(TSRMLS_D);
void cassandra_define_Timestamp(TSRMLS_D);
void cassandra_define_UuidInterface(TSRMLS_D);
void cassandra_define_Uuid(TSRMLS_D);
void cassandra_define_Timeuuid(TSRMLS_D);
void cassandra_define_Varint(TSRMLS_D);

/* Classes */
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_cluster_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_cluster_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_cluster_builder_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ssl_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ssl_builder_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_future_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_future_prepared_statement_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_future_rows_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_future_session_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_future_value_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_future_close_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_session_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_session_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_runtime_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_timeout_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_logic_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_domain_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_invalid_argument_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_server_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_overloaded_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_is_bootstrapping_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_execution_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_truncate_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_write_timeout_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_read_timeout_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_truncate_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_unavailable_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_validation_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_invalid_syntax_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_unauthorized_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_invalid_query_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_configuration_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_already_exists_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_unprepared_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_protocol_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_authentication_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_divide_by_zero_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_range_exception_ce;

extern PHP_CASSANDRA_API zend_class_entry* cassandra_statement_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_simple_statement_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_prepared_statement_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_batch_statement_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_execution_options_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_rows_ce;

void cassandra_define_Cassandra(TSRMLS_D);
void cassandra_define_Cluster(TSRMLS_D);
void cassandra_define_ClusterBuilder(TSRMLS_D);
void cassandra_define_DefaultCluster(TSRMLS_D);
void cassandra_define_Future(TSRMLS_D);
void cassandra_define_FuturePreparedStatement(TSRMLS_D);
void cassandra_define_FutureRows(TSRMLS_D);
void cassandra_define_FutureSession(TSRMLS_D);
void cassandra_define_FutureValue(TSRMLS_D);
void cassandra_define_FutureClose(TSRMLS_D);
void cassandra_define_Session(TSRMLS_D);
void cassandra_define_DefaultSession(TSRMLS_D);
void cassandra_define_SSLOptions(TSRMLS_D);
void cassandra_define_SSLOptionsBuilder(TSRMLS_D);
void cassandra_define_Statement(TSRMLS_D);
void cassandra_define_SimpleStatement(TSRMLS_D);
void cassandra_define_PreparedStatement(TSRMLS_D);
void cassandra_define_BatchStatement(TSRMLS_D);
void cassandra_define_ExecutionOptions(TSRMLS_D);
void cassandra_define_Rows(TSRMLS_D);

extern PHP_CASSANDRA_API zend_class_entry* cassandra_schema_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_schema_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_keyspace_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_keyspace_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_table_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_table_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_column_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_column_ce;

void cassandra_define_Schema(TSRMLS_D);
void cassandra_define_DefaultSchema(TSRMLS_D);
void cassandra_define_Keyspace(TSRMLS_D);
void cassandra_define_DefaultKeyspace(TSRMLS_D);
void cassandra_define_Table(TSRMLS_D);
void cassandra_define_DefaultTable(TSRMLS_D);
void cassandra_define_Column(TSRMLS_D);
void cassandra_define_DefaultColumn(TSRMLS_D);

extern PHP_CASSANDRA_API zend_class_entry* cassandra_type_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_type_scalar_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_type_collection_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_type_set_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_type_map_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_type_custom_ce;

void cassandra_define_Type(TSRMLS_D);
void cassandra_define_TypeScalar(TSRMLS_D);
void cassandra_define_TypeCollection(TSRMLS_D);
void cassandra_define_TypeSet(TSRMLS_D);
void cassandra_define_TypeMap(TSRMLS_D);
void cassandra_define_TypeCustom(TSRMLS_D);

extern int php_le_cassandra_cluster();
extern int php_le_cassandra_session();

#endif /* PHP_CASSANDRA_TYPES_H */
