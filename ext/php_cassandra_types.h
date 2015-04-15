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
  cass_bool_t              use_persistent_sessions;
} cassandra_cluster_builder;

typedef struct {
  zend_object     zval;
  CassSsl*        ssl;
} cassandra_ssl_options;

extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_RuntimeException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_TimeoutException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_LogicException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_InvalidArgumentException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_ServerException;

extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_OverloadedException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_IsBootstrappingException;

extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_TruncateException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_WriteTimeoutException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_ReadTimeoutException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_TruncateException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_UnavailableException;

extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_InvalidSyntaxException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_UnauthorizedException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_InvalidQueryException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_ConfigurationException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_AlreadyExistsException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_UnpreparedException;

extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_ProtocolException;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ce_AuthenticationException;

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
void cassandra_define_CassandraException(TSRMLS_D);
void cassandra_define_CassandraInvalidArgumentException(TSRMLS_D);
void cassandra_define_CassandraDomainException(TSRMLS_D);
void cassandra_define_CassandraLogicException(TSRMLS_D);
void cassandra_define_CassandraRuntimeException(TSRMLS_D);
void cassandra_define_CassandraServerException(TSRMLS_D);
void cassandra_define_CassandraTimeoutException(TSRMLS_D);

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
extern PHP_CASSANDRA_API zend_class_entry* cassandra_cluster_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_default_cluster_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_cluster_builder_ce;
extern PHP_CASSANDRA_API zend_class_entry* cassandra_ssl_options_ce;

PHP_MINIT_FUNCTION(Cluster);
PHP_MINIT_FUNCTION(ClusterBuilder);
PHP_MINIT_FUNCTION(DefaultCluster);
PHP_MINIT_FUNCTION(SSLOptions);

extern int php_le_cassandra_cluster();
extern int php_le_cassandra_session();
extern int php_le_cassandra_ssl();

#endif /* PHP_CASSANDRA_TYPES_H */
