#ifndef PHP_CASSANDRA_MATH_H
#define PHP_CASSANDRA_MATH_H

void import_twos_complement(cass_byte_t *data, size_t size, mpz_t *number);
cass_byte_t *export_twos_complement(mpz_t number, size_t *size);

int php_cassandra_parse_float(char *in, int in_len, cass_float_t *number TSRMLS_DC);
int php_cassandra_parse_bigint(char *in, int in_len, cass_int64_t *number TSRMLS_DC);
int php_cassandra_parse_varint(char *in, int in_len, mpz_t *number TSRMLS_DC);
int php_cassandra_parse_decimal(char *in, int in_len, mpz_t *number, long *scale TSRMLS_DC);

void php_cassandra_format_integer(mpz_t number, char **out, int *out_len);
void php_cassandra_format_decimal(mpz_t number, long scale, char **out, int *out_len);

#endif /* PHP_CASSANDRA_MATH_H */
