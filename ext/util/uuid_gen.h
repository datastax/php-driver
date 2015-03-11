#ifndef PHP_CASSANDRA_UUID_GEN_H
#define PHP_CASSANDRA_UUID_GEN_H

void php_cassandra_uuid_generate_random(CassUuid* out);
void php_cassandra_uuid_generate_time(CassUuid* out);
void php_cassandra_uuid_generate_from_time(long timestamp, CassUuid* out);

#endif /* PHP_CASSANDRA_UUID_GEN_H */
