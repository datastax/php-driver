#ifndef PHP_CASSANDRA_UTIL_HASH_H
#define PHP_CASSANDRA_UTIL_HASH_H

int php_cassandra_hash_object(zval* object, CassValueType type, char** key, int* len);


#endif /* PHP_CASSANDRA_UTIL_HASH_H */
