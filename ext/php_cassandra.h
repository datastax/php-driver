#ifndef PHP_CASSANDRA_H
#define PHP_CASSANDRA_H

#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include <gmp.h>
#include <cassandra.h>
#include <php.h>
#include <Zend/zend_exceptions.h>

/* Define Extension Properties */
#define PHP_CASSANDRA_EXTNAME   "cassandra"
#define PHP_CASSANDRA_EXTVER    "0.1.0"

/* Resources */
#define PHP_CASSANDRA_CLUSTER_RES_NAME    "Cassandra Cluster"
#define PHP_CASSANDRA_SESSION_RES_NAME    "Cassandra Session"
#define PHP_CASSANDRA_FUTURE_RES_NAME     "Cassandra Future"
#define PHP_CASSANDRA_STATEMENT_RES_NAME  "Cassandra Statement"
#define PHP_CASSANDRA_RESULT_RES_NAME     "Cassandra Result"
#define PHP_CASSANDRA_PREPARED_RES_NAME   "Cassandra Prepared Statement"
#define PHP_CASSANDRA_BATCH_RES_NAME      "Cassandra Batch Statement"

extern zend_module_entry cassandra_module_entry;
#define phpext_cassandra_ptr &cassandra_module_entry

#ifdef PHP_WIN32
#    define PHP_CASSANDRA_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#    define PHP_CASSANDRA_API __attribute__ ((visibility("default")))
#else
#    define PHP_CASSANDRA_API
#endif

#ifndef PHP_FE_END
#    define PHP_FE_END { NULL, NULL, NULL, 0, 0 }
#endif

#if ZEND_MODULE_API_NO < 20100525
#    define object_properties_init(value, class_entry) \
              zend_hash_copy(*value.properties, &class_entry->default_properties, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));
#endif

#define SAFE_STR(a) ((a)?a:"")

#ifdef ZTS
#    include "TSRM.h"
#endif

void throw_invalid_argument(zval* object,
                            const char* object_name,
                            const char* expected_type TSRMLS_DC);

#define INVALID_ARGUMENT(object, expected) \
{ \
  throw_invalid_argument(object, #object, #expected TSRMLS_CC); \
  return; \
}

#define INVALID_ARGUMENT_VALUE(object, expected, failed_value) \
{ \
  throw_invalid_argument(object, #object, #expected TSRMLS_CC); \
  return failed_value; \
}

#define ASSERT_SUCCESS(rc) \
{ \
  if (rc != CASS_OK) { \
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC, \
                            "%s", cass_error_desc(rc)); \
    return; \
  } \
}

#define ASSERT_SUCCESS_VALUE(rc, failed_value) \
{ \
  if (rc != CASS_OK) { \
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC, \
                            "%s", cass_error_desc(rc)); \
    return failed_value; \
  } \
}

#include "php_cassandra_types.h"

zend_class_entry* exception_class(CassError rc);

PHP_MINIT_FUNCTION(cassandra);
PHP_MSHUTDOWN_FUNCTION(cassandra);
PHP_RINIT_FUNCTION(cassandra);
PHP_RSHUTDOWN_FUNCTION(cassandra);
PHP_MINFO_FUNCTION(cassandra);

/* Log */
PHP_FUNCTION(cassandra_set_log_level);

/* Util */
PHP_FUNCTION(cassandra_rows_from_result);

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

ZEND_BEGIN_MODULE_GLOBALS(cassandra)
  CassUuidGen*          uuid_gen;
  CassLogLevel          log_level;
  unsigned int          persistent_clusters;
  unsigned int          persistent_sessions;
ZEND_END_MODULE_GLOBALS(cassandra)

#ifdef ZTS
#    define CASSANDRA_G(v) TSRMG(cassandra_globals_id, zend_cassandra_globals *, v)
#else
#    define CASSANDRA_G(v) (cassandra_globals.v)
#endif

#endif /* PHP_CASSANDRA_H */
