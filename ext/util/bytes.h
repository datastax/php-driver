#ifndef PHP_CASSANDRA_BYTES_H
#define PHP_CASSANDRA_BYTES_H

void php_cassandra_bytes_to_hex(const char* bin, int len, char** out, int* out_len);

#endif /* PHP_CASSANDRA_BYTES_H */
