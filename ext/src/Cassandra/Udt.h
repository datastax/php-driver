#ifndef PHP_CASSANDRA_UDT_H
#define PHP_CASSANDRA_UDT_H

int php_cassandra_udt_add(cassandra_udt* udt, zval* object TSRMLS_DC);
int php_cassandra_udt_set(cassandra_udt *udt,
                          const char *name, size_t name_length,
                          zval *object TSRMLS_DC);


#endif /* PHP_CASSANDRA_UDT_H */
