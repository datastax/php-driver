#ifndef PHP_CASSANDRA_FUTURE_H
#define PHP_CASSANDRA_FUTURE_H

int php_cassandra_future_wait_timed(CassFuture* future, zval* timeout);
int php_cassandra_future_is_error(CassFuture* future);

#endif /* PHP_CASSANDRA_FUTURE_H */

