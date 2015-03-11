#ifndef PHP_CASSANDRA_MATH_H
#define PHP_CASSANDRA_MATH_H

int php_cassandra_parse_integer(char* in, int in_len, mpz_t* number);
int php_cassandra_parse_decimal(char* in, int in_len, mpz_t* number, int* scale);

#endif /* PHP_CASSANDRA_MATH_H */
