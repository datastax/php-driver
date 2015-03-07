#ifndef PHP_CASSANDRA_TIMESTAMP_H
#define PHP_CASSANDRA_TIMESTAMP_H

PHP_METHOD(CassandraTimestamp, __construct);
PHP_METHOD(CassandraTimestamp, time);
PHP_METHOD(CassandraTimestamp, microtime);
PHP_METHOD(CassandraTimestamp, toDateTime);
PHP_METHOD(CassandraTimestamp, __toString);

#endif /* PHP_CASSANDRA_TIMESTAMP_H */
