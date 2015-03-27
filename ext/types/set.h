#ifndef PHP_CASSANDRA_SET_H
#define PHP_CASSANDRA_SET_H

int php_cassandra_set_add(cassandra_set* set, zval* object TSRMLS_DC);

PHP_METHOD(CassandraSet, __construct);
PHP_METHOD(CassandraSet, type);
PHP_METHOD(CassandraSet, values);
PHP_METHOD(CassandraSet, add);
PHP_METHOD(CassandraSet, remove);
PHP_METHOD(CassandraSet, has);
/* Countable */
PHP_METHOD(CassandraSet, count);
/* Iterator */
PHP_METHOD(CassandraSet, current);
PHP_METHOD(CassandraSet, key);
PHP_METHOD(CassandraSet, next);
PHP_METHOD(CassandraSet, valid);
PHP_METHOD(CassandraSet, rewind);

#endif /* PHP_CASSANDRA_SET_H */
