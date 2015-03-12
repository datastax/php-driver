#ifndef PHP_CASSANDRA_UTIL_INET_H
#define PHP_CASSANDRA_UTIL_INET_H

void php_cassandra_format_address(CassInet inet, char** out);
int php_cassandra_parse_ip_address(char* in, CassInet* inet);


#endif /* PHP_CASSANDRA_UTIL_INET_H */
