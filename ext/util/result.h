#ifndef PHP_CASSANDRA_RESULT_H
#define PHP_CASSANDRA_RESULT_H

int php_cassandra_get_schema_field(const CassSchemaMeta* metadata, const char* field_name, zval** out TSRMLS_DC);
int php_cassandra_get_result(const CassResult* result, zval** out TSRMLS_DC);

#endif /* PHP_CASSANDRA_RESULT_H */
