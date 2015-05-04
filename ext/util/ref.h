#ifndef PHP_CASSANDRA_REF_H
#define PHP_CASSANDRA_REF_H

cassandra_statement_ref* php_cassandra_new_ref(CassStatement* statement);
cassandra_statement_ref* php_cassandra_add_ref(cassandra_statement_ref* ref);
void                     php_cassandra_del_ref(cassandra_statement_ref** ref_ptr);

#endif /* PHP_CASSANDRA_REF_H */
