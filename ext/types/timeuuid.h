#ifndef PHP_CASSANDRA_TIMEUUID_H
#define PHP_CASSANDRA_TIMEUUID_H

PHP_METHOD(CassandraTimeuuid, __construct);
PHP_METHOD(CassandraTimeuuid, __toString);
PHP_METHOD(CassandraTimeuuid, uuid);
PHP_METHOD(CassandraTimeuuid, version);
PHP_METHOD(CassandraTimeuuid, time);
PHP_METHOD(CassandraTimeuuid, toDateTime);

#endif /* PHP_CASSANDRA_TIMEUUID_H */
