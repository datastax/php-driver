#ifndef PHP_CASSANDRA_UTIL_COLLECTIONS_H
#define PHP_CASSANDRA_UTIL_COLLECTIONS_H

int php_cassandra_hash_object(zval* object, CassValueType type, char** key, int* len);
int php_cassandra_value_type(char* type, CassValueType* value_type);
const char* php_cassandra_type_name(CassValueType value_type);

#endif /* PHP_CASSANDRA_UTIL_COLLECTIONS_H */
