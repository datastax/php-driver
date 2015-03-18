#ifndef PHP_CASSANDRA_SET_H
#define PHP_CASSANDRA_SET_H

PHP_METHOD(CassandraSet, __construct);
PHP_METHOD(CassandraSet, __toString);
PHP_METHOD(CassandraSet, count);
PHP_METHOD(CassandraSet, type);
PHP_METHOD(CassandraSet, toArray);
PHP_METHOD(CassandraSet, add);
PHP_METHOD(CassandraSet, remove);

#endif /* PHP_CASSANDRA_SET_H */
