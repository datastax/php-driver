#ifndef PHP_CASSANDRA_USER_VALUE_H
#define PHP_CASSANDRA_USER_VALUE_H

int php_cassandra_user_type_value_set(cassandra_user_type_value *user_type_value,
                                      const char *name, size_t name_length,
                                      zval *object TSRMLS_DC);


#endif /* PHP_CASSANDRA_USER_VALUE_H */
