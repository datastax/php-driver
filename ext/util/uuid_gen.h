#ifndef PHP_CASSANDRA_UTIL_UUID_GEN_H
#define PHP_CASSANDRA_UTIL_UUID_GEN_H

void php_cassandra_uuid_generate_random(CassUuid *out TSRMLS_DC);
void php_cassandra_uuid_generate_time(CassUuid *out TSRMLS_DC);
void php_cassandra_uuid_generate_from_time(long timestamp, CassUuid *out TSRMLS_DC);

#endif /* PHP_CASSANDRA_UTIL_UUID_GEN_H */
