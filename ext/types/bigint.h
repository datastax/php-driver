#ifndef PHP_CASSANDRA_BIGINT_H
#define PHP_CASSANDRA_BIGINT_H

#include "exceptions\exception.h"

PHP_METHOD(CassandraBigint, __construct);
PHP_METHOD(CassandraBigint, __toString);
PHP_METHOD(CassandraBigint, value);

#endif /* PHP_CASSANDRA_BIGINT_H */
