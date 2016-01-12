#ifndef PHP_CASSANDRA_TYPE_UDT_H
#define PHP_CASSANDRA_TYPE_UDT_H

int php_cassandra_type_udt_add(cassandra_type *type,
                               const char *name, size_t name_length,
                               zval *zsub_type TSRMLS_DC);

#endif /* PHP_CASSANDRA_TYPE_UDT_H */


