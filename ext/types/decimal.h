#ifndef PHP_CASSANDRA_DECIMAL_H
#define PHP_CASSANDRA_DECIMAL_H

#include "exceptions\exception.h"

PHP_METHOD(CassandraDecimal, __construct);
PHP_METHOD(CassandraDecimal, __toString);
PHP_METHOD(CassandraDecimal, value);
PHP_METHOD(CassandraDecimal, scale);

#endif /* PHP_CASSANDRA_DECIMAL_H */
