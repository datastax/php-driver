#ifndef PHP_CASSANDRA_UTIL_FUTURE_H
#define PHP_CASSANDRA_UTIL_FUTURE_H

int  php_cassandra_future_wait_timed(CassFuture *future, zval *timeout TSRMLS_DC);
int  php_cassandra_future_is_error(CassFuture *future TSRMLS_DC);

#endif /* PHP_CASSANDRA_UTIL_FUTURE_H */
