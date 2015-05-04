#ifndef PHP_CASSANDRA_COLLECTION_H
#define PHP_CASSANDRA_COLLECTION_H

int php_cassandra_collection_add(cassandra_collection* collection, zval* object TSRMLS_DC);

PHP_METHOD(CassandraCollection, __construct);
PHP_METHOD(CassandraCollection, type);
PHP_METHOD(CassandraCollection, values);
PHP_METHOD(CassandraCollection, add);
PHP_METHOD(CassandraCollection, get);
PHP_METHOD(CassandraCollection, find);
/* Countable */
PHP_METHOD(CassandraCollection, count);
/* Iterator */
PHP_METHOD(CassandraCollection, current);
PHP_METHOD(CassandraCollection, key);
PHP_METHOD(CassandraCollection, next);
PHP_METHOD(CassandraCollection, valid);
PHP_METHOD(CassandraCollection, rewind);

#endif /* PHP_CASSANDRA_COLLECTION_H */
