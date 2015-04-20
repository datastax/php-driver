#ifndef PHP_CASSANDRA_TYPES_H
#define PHP_CASSANDRA_TYPES_H
typedef struct {
  zend_object  zval;
  cass_float_t value;
} cassandra_float;

typedef struct {
  zend_object  zval;
  cass_int64_t value;
} cassandra_bigint;

typedef struct {
  zend_object  zval;
  cass_int64_t timestamp;
} cassandra_timestamp;

typedef struct {
  zend_object  zval;
  cass_byte_t* data;
  cass_size_t  size;
} cassandra_blob;

typedef struct {
  zend_object  zval;
  mpz_t        value;
} cassandra_varint;

typedef struct {
  zend_object zval;
  mpz_t       value;
  long        scale;
} cassandra_decimal;

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
  LOAD_BALANCING_ROUND_ROBIN = 0,
  LOAD_BALANCING_DC_AWARE_ROUND_ROBIN
} cassandra_load_balancing;

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
  CassConsistency          default_consistency;
  int                      default_page_size;
  zval*                    default_timeout;
  cass_bool_t              persist;
} cassandra_cluster_builder;

typedef struct {
  zend_object       zval;
  CassFuture*       future;
  CassSession*      session;
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

extern PHP_CASSANDRA_API zend_class_entry* cassandra_runtime_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_timeout_exception_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_logic_exception_ce;
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

extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Bigint;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Blob;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Decimal;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Float;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Inet;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Timestamp;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_UuidInterface;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Uuid;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Timeuuid;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Varint;

extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Set;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Map;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_Collection;

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

/* Types */
void cassandra_define_CassandraBigint(TSRMLS_D);
void cassandra_define_CassandraBlob(TSRMLS_D);
void cassandra_define_CassandraCollection(TSRMLS_D);
void cassandra_define_CassandraDecimal(TSRMLS_D);
void cassandra_define_CassandraFloat(TSRMLS_D);
void cassandra_define_CassandraInet(TSRMLS_D);
void cassandra_define_CassandraMap(TSRMLS_D);
void cassandra_define_CassandraSet(TSRMLS_D);
void cassandra_define_CassandraTimestamp(TSRMLS_D);
void cassandra_define_CassandraUuidInterface(TSRMLS_D);
void cassandra_define_CassandraUuid(TSRMLS_D);
void cassandra_define_CassandraTimeuuid(TSRMLS_D);
void cassandra_define_CassandraVarint(TSRMLS_D);

/* Classes */
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_cluster_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_cluster_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_cluster_builder_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ssl_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_future_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_future_session_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_session_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_session_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_exception_ce;

PHP_MINIT_FUNCTION(Cluster);
PHP_MINIT_FUNCTION(ClusterBuilder);
PHP_MINIT_FUNCTION(DefaultCluster);
PHP_MINIT_FUNCTION(Future);
PHP_MINIT_FUNCTION(FutureSession);
PHP_MINIT_FUNCTION(Session);
PHP_MINIT_FUNCTION(DefaultSession);
PHP_MINIT_FUNCTION(SSLOptions);
void cassandra_define_SSLOptionsBuilder(TSRMLS_D);
void cassandra_define_Cassandra(TSRMLS_D);

extern int php_le_cassandra_cluster();
extern int php_le_cassandra_session();

#endif /* PHP_CASSANDRA_TYPES_H */
