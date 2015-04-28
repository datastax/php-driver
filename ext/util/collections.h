#ifndef PHP_CASSANDRA_UTIL_COLLECTIONS_H
#define PHP_CASSANDRA_UTIL_COLLECTIONS_H

#include "exceptions/exception.h"

// SPL should be enabled by default for PHP v5.3+
#if HAVE_SPL
# include <ext/spl/spl_iterators.h>
#else
# error SPL must be enabled in order to build the driver
#endif
#include <Zend/zend_interfaces.h>

int php_cassandra_hash_object(zval* object, CassValueType type, char** key, int* len TSRMLS_DC);
int php_cassandra_value_type(char* type, CassValueType* value_type TSRMLS_DC);
int php_cassandra_validate_object(zval* object, CassValueType type TSRMLS_DC);

const char* php_cassandra_type_name(CassValueType value_type);

int php_cassandra_collection_from_set(cassandra_set* set, CassCollection** collection_ptr TSRMLS_DC);
int php_cassandra_collection_from_collection(cassandra_collection* coll, CassCollection** collection_ptr TSRMLS_DC);
int php_cassandra_collection_from_map(cassandra_map* map, CassCollection** collection_ptr TSRMLS_DC);

#endif /* PHP_CASSANDRA_UTIL_COLLECTIONS_H */
