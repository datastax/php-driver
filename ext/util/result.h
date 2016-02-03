#ifndef PHP_CASSANDRA_RESULT_H
#define PHP_CASSANDRA_RESULT_H

int php_cassandra_get_keyspace_field(const CassKeyspaceMeta *metadata, const char *field_name, php5to7_zval *out TSRMLS_DC);
int php_cassandra_get_table_field(const CassTableMeta *metadata, const char *field_name, php5to7_zval *out TSRMLS_DC);
int php_cassandra_get_column_field(const CassColumnMeta *metadata, const char *field_name, php5to7_zval *out TSRMLS_DC);

int php_cassandra_get_result(const CassResult *result, php5to7_zval *out TSRMLS_DC);

#endif /* PHP_CASSANDRA_RESULT_H */
