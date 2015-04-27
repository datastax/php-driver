#ifndef PHP_CASSANDRA_UUID_INTERFACE_H
#define PHP_CASSANDRA_UUID_INTERFACE_H

extern zend_class_entry* cassandra_ce_UuidInterface;

PHP_METHOD(CassandraUuidInterface, uuid);
PHP_METHOD(CassandraUuidInterface, version);

#endif /* PHP_CASSANDRA_UUID_INTERFACE_H */
