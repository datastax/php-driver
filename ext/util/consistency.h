#ifndef PHP_CASSANDRA_CONSISTENCY_H
#define PHP_CASSANDRA_CONSISTENCY_H

int php_cassandra_get_consistency(zval *consistency, long *result TSRMLS_DC);
int php_cassandra_get_serial_consistency(zval *serial_consistency, long *result TSRMLS_DC);

#endif /* PHP_CASSANDRA_CONSISTENCY_H */
