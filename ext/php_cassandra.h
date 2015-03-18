#ifndef PHP_CASSANDRA_H
#define PHP_CASSANDRA_H
#include <gmp.h>
#include <cassandra.h>

/* Define Extension Properties */
#define PHP_CASSANDRA_EXTNAME   "cassandra"
#define PHP_CASSANDRA_EXTVER    "0.1.0"

/* Resources */
#define PHP_CASSANDRA_CLUSTER_RES_NAME    "Cassandra Cluster"
#define PHP_CASSANDRA_SESSION_RES_NAME    "Cassandra Session"
#define PHP_CASSANDRA_FUTURE_RES_NAME     "Cassandra Future"
#define PHP_CASSANDRA_STATEMENT_RES_NAME  "Cassandra Statement"
#define PHP_CASSANDRA_RESULT_RES_NAME     "Cassandra Result"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

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
  cass_int64_t timestamp;
} cassandra_timestamp;

typedef struct {
  zend_object zval;
  CassBytes   bytes;
} cassandra_blob;

typedef struct {
  zend_object zval;
  mpz_t       value;
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

PHP_MINIT_FUNCTION(cassandra);
PHP_MSHUTDOWN_FUNCTION(cassandra);
PHP_RINIT_FUNCTION(cassandra);
PHP_RSHUTDOWN_FUNCTION(cassandra);
PHP_MINFO_FUNCTION(cassandra);

/* Util */
PHP_FUNCTION(cassandra_error_message);
PHP_FUNCTION(cassanrda_rows_from_result);

/* CassCluster */
PHP_FUNCTION(cassandra_cluster_new);
PHP_FUNCTION(cassandra_cluster_free);
PHP_FUNCTION(cassandra_cluster_set_load_balance_round_robin);
PHP_FUNCTION(cassandra_cluster_set_load_balance_dc_aware);
PHP_FUNCTION(cassandra_cluster_set_token_aware_routing);
PHP_FUNCTION(cassandra_cluster_set_credentials);
PHP_FUNCTION(cassandra_cluster_set_contact_points);

/* CassSession */
PHP_FUNCTION(cassandra_session_new);
PHP_FUNCTION(cassandra_session_free);
PHP_FUNCTION(cassandra_session_connect);
PHP_FUNCTION(cassandra_session_connect_keyspace);
PHP_FUNCTION(cassandra_session_execute);

/* CassFuture */
PHP_FUNCTION(cassandra_future_free);
PHP_FUNCTION(cassandra_future_wait);
PHP_FUNCTION(cassandra_future_wait_timed);
PHP_FUNCTION(cassandra_future_error_code);
PHP_FUNCTION(cassandra_future_error_message);
PHP_FUNCTION(cassandra_future_get_result);

/* CassResult */
PHP_FUNCTION(cassandra_result_free);
PHP_FUNCTION(cassandra_result_row_count);

/* CassStatement */
PHP_FUNCTION(cassandra_statement_new);
PHP_FUNCTION(cassandra_statement_free);

/* Exceptions */
void cassandra_define_CassandraException(TSRMLS_D);
void cassandra_define_CassandraInvalidArgumentException(TSRMLS_D);

/* Types */
void cassandra_define_CassandraBigint(TSRMLS_D);
void cassandra_define_CassandraBlob(TSRMLS_D);
void cassandra_define_CassandraDecimal(TSRMLS_D);
void cassandra_define_CassandraInet(TSRMLS_D);
void cassandra_define_CassandraTimestamp(TSRMLS_D);
void cassandra_define_CassandraUuidInterface(TSRMLS_D);
void cassandra_define_CassandraUuid(TSRMLS_D);
void cassandra_define_CassandraTimeuuid(TSRMLS_D);
void cassandra_define_CassandraVarint(TSRMLS_D);

ZEND_BEGIN_MODULE_GLOBALS(cassandra)
  CassUuidGen* uuid_gen;
ZEND_END_MODULE_GLOBALS(cassandra)


#ifdef ZTS
#define CASSANDRA_G(v) TSRMG(cassandra_globals_id, zend_cassandra_globals *, v)
#else
#define CASSANDRA_G(v) (cassandra_globals.v)
#endif

#endif /* PHP_CASSANDRA_H */
