#ifndef PHP_CASSANDRA_RESULT_H
#define PHP_CASSANDRA_RESULT_H

int php_cassandra_get_result(const CassResult* result, zval** out TSRMLS_DC);

#endif /* PHP_CASSANDRA_RESULT_H */
