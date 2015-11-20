#ifndef PHP_CASSANDRA_RESULT_H
#define PHP_CASSANDRA_RESULT_H

#if CURRENT_DRIVER_VERSION >= DRIVER_VERSION(2, 2, 0)
int php_cassandra_get_keyspace_field(const CassKeyspaceMeta* metadata, const char* field_name, zval** out TSRMLS_DC);
int php_cassandra_get_table_field(const CassTableMeta* metadata, const char* field_name, zval** out TSRMLS_DC);
int php_cassandra_get_column_field(const CassColumnMeta* metadata, const char* field_name, zval** out TSRMLS_DC);
#else
int php_cassandra_get_schema_field(const CassSchemaMeta* metadata, const char* field_name, zval** out TSRMLS_DC);
#define php_cassandra_get_keyspace_field php_cassandra_get_schema_field
#define php_cassandra_get_table_field php_cassandra_get_schema_field
#define php_cassandra_get_column_field php_cassandra_get_schema_field
#endif

int php_cassandra_get_result(const CassResult* result, zval** out TSRMLS_DC);

#endif /* PHP_CASSANDRA_RESULT_H */
