#ifndef PHP_CASSANDRA_UUID_H
#define PHP_CASSANDRA_UUID_H

#include "exceptions/exception.h"
#include "uuid_interface.h"

PHP_METHOD(CassandraUuid, __construct);
PHP_METHOD(CassandraUuid, __toString);
PHP_METHOD(CassandraUuid, uuid);
PHP_METHOD(CassandraUuid, version);

#endif /* PHP_CASSANDRA_UUID_H */
