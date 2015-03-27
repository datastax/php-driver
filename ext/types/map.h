#ifndef PHP_CASSANDRA_MAP_H
#define PHP_CASSANDRA_MAP_H

int php_cassandra_map_set(cassandra_map* map, zval* zkey, zval* zvalue TSRMLS_DC);

PHP_METHOD(CassandraMap, __construct);
PHP_METHOD(CassandraMap, keyType);
PHP_METHOD(CassandraMap, keys);
PHP_METHOD(CassandraMap, valueType);
PHP_METHOD(CassandraMap, values);
PHP_METHOD(CassandraMap, set);
PHP_METHOD(CassandraMap, get);
PHP_METHOD(CassandraMap, remove);
PHP_METHOD(CassandraMap, has);
/* Countable */
PHP_METHOD(CassandraMap, count);
/* Iterator */
PHP_METHOD(CassandraMap, current);
PHP_METHOD(CassandraMap, key);
PHP_METHOD(CassandraMap, next);
PHP_METHOD(CassandraMap, valid);
PHP_METHOD(CassandraMap, rewind);
/* ArrayAccess */
PHP_METHOD(CassandraMap, offsetSet);
PHP_METHOD(CassandraMap, offsetGet);
PHP_METHOD(CassandraMap, offsetUnset);
PHP_METHOD(CassandraMap, offsetExists);

#endif /* PHP_CASSANDRA_MAP_H */
