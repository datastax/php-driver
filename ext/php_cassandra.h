#ifndef PHP_CASSANDRA_H
#define PHP_CASSANDRA_H

#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include <gmp.h>
#include <cassandra.h>
#include <uv.h>
#include <php.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#if HAVE_SPL
#    include <ext/spl/spl_iterators.h>
#    include <ext/spl/spl_exceptions.h>
#else
#    error SPL must be enabled in order to build the driver
#endif

#include "version.h"

/* Resources */
#define PHP_CASSANDRA_CLUSTER_RES_NAME    "Cassandra Cluster"
#define PHP_CASSANDRA_SESSION_RES_NAME    "Cassandra Session"

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

zend_class_entry* exception_class(CassError rc);

void throw_invalid_argument(zval* object,
                            const char* object_name,
                            const char* expected_type TSRMLS_DC);

#define INVALID_ARGUMENT(object, expected) \
{ \
  throw_invalid_argument(object, #object, expected TSRMLS_CC); \
  return; \
}

#define INVALID_ARGUMENT_VALUE(object, expected, failed_value) \
{ \
  throw_invalid_argument(object, #object, expected TSRMLS_CC); \
  return failed_value; \
}

#define ASSERT_SUCCESS_BLOCK(rc, block) \
{ \
  if (rc != CASS_OK) { \
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC, \
                            "%s", cass_error_desc(rc)); \
    block \
  } \
}

#define ASSERT_SUCCESS(rc) ASSERT_SUCCESS_BLOCK(rc, return;)

#define ASSERT_SUCCESS_VALUE(rc, value) ASSERT_SUCCESS_BLOCK(rc, return value;)

#include "php_cassandra_types.h"

PHP_MINIT_FUNCTION(cassandra);
PHP_MSHUTDOWN_FUNCTION(cassandra);
PHP_RINIT_FUNCTION(cassandra);
PHP_RSHUTDOWN_FUNCTION(cassandra);
PHP_MINFO_FUNCTION(cassandra);

ZEND_BEGIN_MODULE_GLOBALS(cassandra)
  CassUuidGen*          uuid_gen;
  unsigned int          persistent_clusters;
  unsigned int          persistent_sessions;
ZEND_END_MODULE_GLOBALS(cassandra)

#ifdef ZTS
#    define CASSANDRA_G(v) TSRMG(cassandra_globals_id, zend_cassandra_globals *, v)
#else
#    define CASSANDRA_G(v) (cassandra_globals.v)
#endif

#endif /* PHP_CASSANDRA_H */
