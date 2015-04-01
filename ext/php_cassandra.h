#ifndef PHP_CASSANDRA_H
#define PHP_CASSANDRA_H
#include <gmp.h>
#include <cassandra.h>

/* Define Extension Properties */
#define PHP_CASSANDRA_EXTNAME   "cassandra"
#define PHP_CASSANDRA_EXTVER    "0.1.0"

/* Resources */
#define PHP_CASSANDRA_CLUSTER_RES_NAME    "Cassandra Cluster"
#define PHP_CASSANDRA_SSL_RES_NAME        "Cassandra Ssl"
#define PHP_CASSANDRA_SESSION_RES_NAME    "Cassandra Session"
#define PHP_CASSANDRA_FUTURE_RES_NAME     "Cassandra Future"
#define PHP_CASSANDRA_STATEMENT_RES_NAME  "Cassandra Statement"
#define PHP_CASSANDRA_RESULT_RES_NAME     "Cassandra Result"
#define PHP_CASSANDRA_PREPARED_RES_NAME   "Cassandra Prepared Statement"
#define PHP_CASSANDRA_BATCH_RES_NAME      "Cassandra Batch Statement"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

extern zend_module_entry cassandra_module_entry;
#define phpext_cassandra_ptr &cassandra_module_entry

#ifdef PHP_WIN32
#    define PHP_CASSANDRA_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#    define PHP_CASSANDRA_API __attribute__ ((visibility("default")))
#else
#    define PHP_CASSANDRA_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

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

PHP_MINIT_FUNCTION(cassandra);
PHP_MSHUTDOWN_FUNCTION(cassandra);
PHP_RINIT_FUNCTION(cassandra);
PHP_RSHUTDOWN_FUNCTION(cassandra);
PHP_MINFO_FUNCTION(cassandra);

/* Log */
PHP_FUNCTION(cassandra_set_log_level);

/* Util */
PHP_FUNCTION(cassanrda_rows_from_result);

/* CassCluster */
PHP_FUNCTION(cassandra_cluster_new);
PHP_FUNCTION(cassandra_cluster_free);
PHP_FUNCTION(cassandra_cluster_set_load_balance_round_robin);
PHP_FUNCTION(cassandra_cluster_set_load_balance_dc_aware);
PHP_FUNCTION(cassandra_cluster_set_token_aware_routing);
PHP_FUNCTION(cassandra_cluster_set_credentials);
PHP_FUNCTION(cassandra_cluster_set_contact_points);
PHP_FUNCTION(cassandra_cluster_set_ssl);

/* CassSsl */
PHP_FUNCTION(cassandra_ssl_new);
PHP_FUNCTION(cassandra_ssl_free);
PHP_FUNCTION(cassandra_ssl_add_trusted_cert);
PHP_FUNCTION(cassandra_ssl_set_cert);
PHP_FUNCTION(cassandra_ssl_set_private_key);
PHP_FUNCTION(cassandra_ssl_set_verify_flags);

/* CassSession */
PHP_FUNCTION(cassandra_session_new);
PHP_FUNCTION(cassandra_session_free);
PHP_FUNCTION(cassandra_session_connect);
PHP_FUNCTION(cassandra_session_connect_keyspace);
PHP_FUNCTION(cassandra_session_execute);
PHP_FUNCTION(cassandra_session_prepare);
PHP_FUNCTION(cassandra_session_execute_batch);
PHP_FUNCTION(cassandra_session_close);

/* CassFuture */
PHP_FUNCTION(cassandra_future_free);
PHP_FUNCTION(cassandra_future_wait);
PHP_FUNCTION(cassandra_future_wait_timed);
PHP_FUNCTION(cassandra_future_get_result);
PHP_FUNCTION(cassandra_future_get_prepared);

/* CassResult */
PHP_FUNCTION(cassandra_result_free);
PHP_FUNCTION(cassandra_result_row_count);
PHP_FUNCTION(cassandra_result_has_more_pages);

/* CassStatement */
PHP_FUNCTION(cassandra_statement_new);
PHP_FUNCTION(cassandra_statement_free);
PHP_FUNCTION(cassandra_statement_bind);
PHP_FUNCTION(cassandra_statement_set_consistency);
PHP_FUNCTION(cassandra_statement_set_paging_size);
PHP_FUNCTION(cassandra_statement_set_serial_consistency);
PHP_FUNCTION(cassandra_statement_set_paging_state);

/* CassPrepared */
PHP_FUNCTION(cassandra_prepared_free);
PHP_FUNCTION(cassandra_prepared_bind);

/* CassBatch */
PHP_FUNCTION(cassandra_batch_new);
PHP_FUNCTION(cassandra_batch_free);
PHP_FUNCTION(cassandra_batch_set_consistency);
PHP_FUNCTION(cassandra_batch_add_statement);

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

ZEND_BEGIN_MODULE_GLOBALS(cassandra)
  CassUuidGen*          uuid_gen;
  CassLogLevel          log_level;
ZEND_END_MODULE_GLOBALS(cassandra)

#ifdef ZTS
#define CASSANDRA_G(v) TSRMG(cassandra_globals_id, zend_cassandra_globals *, v)
#else
#define CASSANDRA_G(v) (cassandra_globals.v)
#endif

#endif /* PHP_CASSANDRA_H */
