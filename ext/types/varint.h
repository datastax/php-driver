#ifndef PHP_CASSANDRA_VARINT_H
#define PHP_CASSANDRA_VARINT_H

#include "exceptions/exception.h"

PHP_METHOD(CassandraVarint, __construct);
PHP_METHOD(CassandraVarint, __toString);
PHP_METHOD(CassandraVarint, value);

#endif /* PHP_CASSANDRA_VARINT_H */
