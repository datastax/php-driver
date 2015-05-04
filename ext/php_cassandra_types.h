#ifndef PHP_CASSANDRA_TYPES_H
#define PHP_CASSANDRA_TYPES_H

typedef enum {
  CASSANDRA_BIGINT,
  CASSANDRA_DECIMAL,
  CASSANDRA_FLOAT,
  CASSANDRA_VARINT,
} cassandra_numeric_type;

#define NUMERIC_FIELDS \
  zend_object  zval; \
  cassandra_numeric_type type;

typedef struct {
  NUMERIC_FIELDS
} cassandra_numeric;

typedef struct {
  NUMERIC_FIELDS
  cass_int64_t value;
} cassandra_bigint;

typedef struct {
  NUMERIC_FIELDS
  mpz_t       value;
  long        scale;
} cassandra_decimal;

typedef struct {
  NUMERIC_FIELDS
  cass_float_t value;
} cassandra_float;

typedef struct {
  NUMERIC_FIELDS
  mpz_t        value;
} cassandra_varint;

#undef NUMERIC_FIELDS

typedef struct {
  zend_object  zval;
  cass_int64_t timestamp;
} cassandra_timestamp;

typedef struct {
  zend_object  zval;
  cass_byte_t* data;
  size_t       size;
} cassandra_blob;

typedef struct {
  zend_object zval;
  CassUuid    uuid;
} cassandra_uuid;

typedef struct {
  zend_object zval;
  CassInet    inet;
} cassandra_inet;

typedef struct {
  zend_object   zval;
  CassValueType type;
  HashTable     values;
  int           pos;
} cassandra_set;

typedef struct {
  zend_object   zval;
  CassValueType key_type;
  HashTable     keys;
  CassValueType value_type;
  HashTable     values;
} cassandra_map;

typedef struct {
  zend_object   zval;
  CassValueType type;
  HashTable     values;
} cassandra_collection;

typedef struct {
  zend_object     zval;
  CassCluster*    cluster;
  CassConsistency default_consistency;
  int             default_page_size;
  zval*           default_timeout;
  cass_bool_t     persist;
  char*           hash_key;
  int             hash_key_len;
} cassandra_cluster;

typedef enum {
  CASSANDRA_SIMPLE_STATEMENT,
  CASSANDRA_PREPARED_STATEMENT,
  CASSANDRA_BATCH_STATEMENT,
} cassandra_statement_type;

#define STATEMENT_FIELDS \
  zend_object  zval; \
  cassandra_statement_type type;

typedef struct {
  STATEMENT_FIELDS
} cassandra_statement;

typedef struct {
  STATEMENT_FIELDS
  char* cql;
} cassandra_simple_statement;

typedef struct {
  STATEMENT_FIELDS
  const CassPrepared* prepared;
} cassandra_prepared_statement;

typedef struct {
  STATEMENT_FIELDS
  CassBatchType batch_type;
  HashTable* statements;
} cassandra_batch_statement;

#undef STATEMENT_FIELDS

typedef struct {
  zval* statement;
  zval* arguments;
} cassandra_batch_statement_entry;

typedef struct {
  zend_object zval;
  long        consistency;
  long        serial_consistency;
  long        page_size;
  zval*       timeout;
  zval*       arguments;
} cassandra_execution_options;

typedef enum {
  LOAD_BALANCING_ROUND_ROBIN = 0,
  LOAD_BALANCING_DC_AWARE_ROUND_ROBIN
} cassandra_load_balancing;

typedef struct {
  size_t         count;
  CassStatement* statement;
} cassandra_statement_ref;

typedef struct {
  zend_object              zval;
  cassandra_statement_ref* statement;
  zval*                    session;
  zval*                    rows;
  const CassResult*        result;
} cassandra_rows;

typedef struct {
  zend_object              zval;
  cassandra_statement_ref* statement;
  zval*                    session;
  zval*                    rows;
  CassFuture*              future;
} cassandra_future_rows;

typedef struct {
  zend_object              zval;
  char*                    contact_points;
  int                      port;
  cassandra_load_balancing load_balancing_policy;
  char*                    local_dc;
  unsigned int             used_hosts_per_remote_dc;
  cass_bool_t              allow_remote_dcs_for_local_cl;
  cass_bool_t              use_token_aware_routing;
  char*                    username;
  char*                    password;
  unsigned int             connect_timeout;
  unsigned int             request_timeout;
  zval*                    ssl_options;
  long                     default_consistency;
  int                      default_page_size;
  zval*                    default_timeout;
  cass_bool_t              persist;
} cassandra_cluster_builder;

typedef struct {
  zend_object zval;
  CassFuture* future;
  zval*       prepared_statement;
} cassandra_future_prepared_statement;

typedef struct {
  zend_object zval;
  zval*       value;
} cassandra_future_value;

typedef struct {
  zend_object zval;
  CassFuture* future;
} cassandra_future_close;

typedef struct {
  zend_object       zval;
  CassFuture*       future;
  CassSession*      session;
  zval*             default_session;
  cass_bool_t       persist;
  char*             hash_key;
  int               hash_key_len;
  char*             exception_message;
  CassError         exception_code;
} cassandra_future_session;

typedef struct {
  CassFuture*  future;
  CassSession* session;
} cassandra_psession;

typedef struct {
  zend_object  zval;
  CassSession* session;
  cass_bool_t  persist;
} cassandra_session;

typedef struct {
  zend_object     zval;
  CassSsl*        ssl;
} cassandra_ssl;

typedef struct {
  zend_object zval;
  int         flags;
  char**      trusted_certs;
  int         trusted_certs_cnt;
  char*       client_cert;
  char*       private_key;
  char*       passphrase;
} cassandra_ssl_builder;


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

extern int php_le_cassandra_cluster();
extern int php_le_cassandra_session();

#endif /* PHP_CASSANDRA_TYPES_H */
