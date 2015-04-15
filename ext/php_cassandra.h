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
#define PHP_CASSANDRA_SSL_RES_NAME        "Cassandra Ssl"
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

#ifdef ZTS
#    include "TSRM.h"
#endif

#if ZEND_MODULE_API_NO >= 20100525
static const char* _cls_name = NULL;
#else
static char* _cls_name = NULL;
#endif
static zend_uint _cls_len;

#define INVALID_ARGUMENT(object, expected) \
  ({ \
    if (Z_TYPE_P(object) == IS_OBJECT) { \
      Z_OBJ_HANDLER_P(object, get_class_name)(object, &_cls_name, &_cls_len, 0 TSRMLS_CC); \
      if (_cls_name) { \
        zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, \
          #object " must be " expected ", an instance of %.*s given", _cls_len, _cls_name); \
        efree((void *) _cls_name); \
      } else { \
        zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, \
          #object " must be " expected ", an instance of Unknown Class given"); \
      } \
    } else { \
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, \
        #object " must be " expected ", %Z given", object); \
    } \
    return; \
  })

#define ASSERT_SUCCESS(rc) \
  ({ \
    if (rc != CASS_OK) {\
      zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC, \
        "%s", cass_error_desc(rc)); \
      return; \
    } \
  })

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

/* CassCluster */
PHP_FUNCTION(cassandra_cluster_new);
PHP_FUNCTION(cassandra_cluster_free);
PHP_FUNCTION(cassandra_cluster_set_load_balance_round_robin);
PHP_FUNCTION(cassandra_cluster_set_load_balance_dc_aware);
PHP_FUNCTION(cassandra_cluster_set_token_aware_routing);
PHP_FUNCTION(cassandra_cluster_set_credentials);
PHP_FUNCTION(cassandra_cluster_set_contact_points);
PHP_FUNCTION(cassandra_cluster_set_port);
PHP_FUNCTION(cassandra_cluster_set_connect_timeout);
PHP_FUNCTION(cassandra_cluster_set_request_timeout);
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

ZEND_BEGIN_MODULE_GLOBALS(cassandra)
  CassUuidGen*          uuid_gen;
  CassLogLevel          log_level;
  unsigned int          persistent_clusters;
ZEND_END_MODULE_GLOBALS(cassandra)

#ifdef ZTS
#    define CASSANDRA_G(v) TSRMG(cassandra_globals_id, zend_cassandra_globals *, v)
#else
#    define CASSANDRA_G(v) (cassandra_globals.v)
#endif

#endif /* PHP_CASSANDRA_H */
