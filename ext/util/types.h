#ifndef PHP_CASSANDRA_UTIL_TYPES_H
#define PHP_CASSANDRA_UTIL_TYPES_H

#define PHP_CASSANDRA_SCALAR_TYPES_MAP(XX) \
  XX(ascii, CASS_VALUE_TYPE_ASCII) \
  XX(bigint, CASS_VALUE_TYPE_BIGINT) \
  XX(blob, CASS_VALUE_TYPE_BLOB) \
  XX(boolean, CASS_VALUE_TYPE_BOOLEAN) \
  XX(counter, CASS_VALUE_TYPE_COUNTER) \
  XX(decimal, CASS_VALUE_TYPE_DECIMAL) \
  XX(double, CASS_VALUE_TYPE_DOUBLE) \
  XX(float, CASS_VALUE_TYPE_FLOAT) \
  XX(int, CASS_VALUE_TYPE_INT) \
  XX(text, CASS_VALUE_TYPE_TEXT) \
  XX(timestamp, CASS_VALUE_TYPE_TIMESTAMP) \
  XX(uuid, CASS_VALUE_TYPE_UUID) \
  XX(varchar, CASS_VALUE_TYPE_VARCHAR) \
  XX(varint, CASS_VALUE_TYPE_VARINT) \
  XX(timeuuid, CASS_VALUE_TYPE_TIMEUUID) \
  XX(inet, CASS_VALUE_TYPE_INET)

php5to7_zval php_cassandra_type_scalar(CassValueType type TSRMLS_DC);
const char *php_cassandra_scalar_type_name(CassValueType type TSRMLS_DC);
php5to7_zval php_cassandra_type_set(CassValueType type TSRMLS_DC);
php5to7_zval php_cassandra_type_collection(CassValueType type TSRMLS_DC);
php5to7_zval php_cassandra_type_map(CassValueType key_type,
                                    CassValueType value_type TSRMLS_DC);
php5to7_zval php_cassandra_type_custom(char *name TSRMLS_DC);
int php_cassandra_parse_column_type(const char   *validator,
                                    size_t        validator_len,
                                    int          *reversed_out,
                                    int          *frozen_out,
                                    php5to7_zval *type_out TSRMLS_DC);

void php_cassandra_scalar_init(INTERNAL_FUNCTION_PARAMETERS);

#endif /* PHP_CASSANDRA_UTIL_TYPES_H */
