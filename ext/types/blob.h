#ifndef PHP_CASSANDRA_BLOB_H
#define PHP_CASSANDRA_BLOB_H

#include "exceptions\exception.h"

PHP_METHOD(CassandraBlob, __construct);
PHP_METHOD(CassandraBlob, __toString);
PHP_METHOD(CassandraBlob, bytes);

#endif /* PHP_CASSANDRA_BLOB_H */
