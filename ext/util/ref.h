#ifndef PHP_CASSANDRA_REF_H
#define PHP_CASSANDRA_REF_H

cassandra_ref *php_cassandra_new_ref(void *data, cassandra_free_function destructor);
cassandra_ref *php_cassandra_add_ref(cassandra_ref *ref);
void           php_cassandra_del_ref(cassandra_ref **ref_ptr);

#endif /* PHP_CASSANDRA_REF_H */
