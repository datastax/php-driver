#include "../php_cassandra.h"
#include <errno.h>
#include <stdlib.h>
#include <gmp.h>
#include <math.h>
#include "util/math.h"

extern zend_class_entry *cassandra_ce_InvalidArgumentException;

int
php_cassandra_parse_integer(char* in, int in_len, mpz_t* number)
{
  int point = 0;
  int base = 10;

  int i, digit;

  //  Determine the sign of the number.
  int negative = 0;
  if (in[point] == '+') {
    point++;
  } else if (in[point] == '-') {
    point++;
    negative = 1;
  }

  if (in[point] == '0') {
    switch(in[point + 1]) {
    case 'b':
      point += 2;
      base = 2;
      break;
    case 'x':
      point += 2;
      base = 16;
      break;
    default:
      base = 8;
      break;
    }
  }

  if (in_len <= 15) {
    long n;
    char* end;
    errno = 0;

    n = strtol(&in[point], &end, base);

    if (negative)
      n = n * -1;

    if (errno) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid integer value: %s", in);
      return 0;
    }

    if (end != &in[in_len]) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Non digit characters were found in value: '%s'", in);
      return 0;
    }

    if (end == &in[point]) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "No digits were found in value: \"%s\"", in);
      return 0;
    }

    mpz_set_si(*number, n);

    return 1;
  }

  if (base != 10) {
    point += 2;
  }

  if (mpz_set_str(*number, &in[point], base) == -1) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Invalid integer value: \"%s\", base: %d", in, base);
    return 0;
  }

  if (negative)
    mpz_neg(*number, *number);

  return 1;
}

int
php_cassandra_parse_decimal(char* in, int in_len, mpz_t* number, int* scale)
{
  //  start is the index into the char array where the significand starts
  int start = 0;
  //  point is the index into the char array where the exponent starts
  //  (or, if there is no exponent, this is equal to end)
  int point = 0;
  //  dot is the index into the char array where the decimal point is
  //  found, or -1 if there is no decimal point
  int dot = -1;
  //  out will be storing the string representation of the integer part
  //  of the decimal value
  char* out = (char*) ecalloc((in_len + 1), sizeof(char));
  //  holds length of the formatted integer number
  int out_len = 0;

  //  The following examples show what these variables mean.  Note that
  //  point and dot don't yet have the correct values, they will be
  //  properly assigned in a loop later on in this method.
  //
  //  Example 1
  //
  //         +  1  0  2  .  4  6  9
  //  __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
  //
  //  offset = 2, in_len = 8, start = 3, dot = 6, point = end = 10
  //
  //  Example 2
  //
  //         +  2  3  4  .  6  1  3  E  -  1
  //  __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
  //
  //  offset = 2, in_len = 11, start = 3, dot = 6, point = 10, end = 13
  //
  //  Example 3
  //
  //         -  1  2  3  4  5  e  7
  //  __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
  //
  //  offset = 2, in_len = 8, start = 3, dot = -1, point = 8, end = 10

  //  Determine the sign of the number.
  int negative = 0;
  if (in[start] == '+') {
    start++;
    point++;
  } else if (in[start] == '-') {
    start++;
    point++;
    negative = 1;
  }

  if (in[point] == '0') {
    if (in[point + 1] == 'b')
      point += 2;
    else if (in[point + 1] == 'x')
      point += 2;
  }

  //  Check each character looking for the decimal point and the
  //  start of the exponent.
  while (point < in_len) {
    char c = in[point];

    if (c == '.') {
      // If dot != -1 then we've seen more than one decimal point.
      if (dot != -1) {
        zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Multiple '.' (dots) in the number \"%s\"", in);
        return 0;
      }

      dot = point;
    }
    // Break when we reach the start of the exponent.
    else if (c == 'e' || c == 'E')
      break;
    // Throw an exception if the character was not a decimal or an
    // exponent and is not a hexadecimal digit.
    else if (!isxdigit(c)) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Unrecognized character '%c' at %d", c, point);
      return 0;
    }

    point++;
  }

  // Prepend a negative sign if necessary.
  if (negative)
    out[0] = '-';

  if (dot != -1) {
    // If there was a decimal we must combine the two parts that
    // contain only digits and we must set the scale properly.
    memcpy(&out[negative], &in[start], dot - start);
    memcpy(&out[negative + dot - start], &in[dot + 1], point - dot);

    out_len = point - start + negative;
    *scale = point - 1 - dot;
  } else {
    // If there was no decimal then the unscaled value is just the number
    // formed from all the digits and the scale is zero.
    memcpy(&out[negative], &in[start], point - start);
    out_len = point - start + negative;
    *scale = 0;
  }

  if (out_len == 0) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "No digits seen in value: \"%s\"", in);
    return 0;
  }

  int ok = php_cassandra_parse_integer(out, out_len, number);

  if (!ok) {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Unable to extract integer part of decimal value: \"%s\", %s", in, out);
    efree(out);
    return 0;
  }
  efree(out);

  // Now parse exponent.
  // If point < end that means we broke out of the previous loop when we
  // saw an 'e' or an 'E'.
  if (point < in_len) {
    point++;
    // Ignore a '+' sign.
    if (in[point] == '+')
      point++;

    // Throw an exception if there were no digits found after the 'e'
    // or 'E'.
    if (point >= in_len) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "No exponent following e or E in value: \"%s\"", in);
      return 0;
    }

    int diff;

    if (!sscanf(&in[point], "%d", &diff)) {
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Malformed exponent in value: \"%s\"", in);
      return 0;
    }

    *scale = *scale - diff;
  }

  return 1;
}

void
php_cassandra_format_integer(mpz_t number, char** out, int* out_len)
{
  size_t len;
  char* tmp;

  len = mpz_sizeinbase(number, 10);
  if (mpz_sgn(number) < 0)
    len++;

  tmp = (char*) emalloc((len + 1) * sizeof(char));
  mpz_get_str(tmp, 10, number);

  if (tmp[len - 1] == '\0') {
    len--;
  } else {
    tmp[len] = '\0';
  }

  *out     = tmp;
  *out_len = len;
}


void
php_cassandra_format_decimal(mpz_t number, long scale, char** out, int* out_len)
{
  if (scale == 0) {
    php_cassandra_format_integer(number, out, out_len);
    return;
  }

  char* tmp;
  size_t total = 0;
  size_t len   = mpz_sizeinbase(number, 10);
  int negative = 0;

  if (mpz_sgn(number) < 0)
    negative = 1;

  int point = len - scale;

  if (scale >= 0 && (point - 1) >= -6) {
    if (point <= 0) {
      // current position
      int i = 0;
      // absolute length + negative sign + point sign + leading zeroes
      total = len + negative + 2 + (point * -1);
      tmp   = (char*) emalloc((total + 1) * sizeof(char));

      if (negative)
        tmp[i++] = '-';

      tmp[i++] = '0';
      tmp[i++] = '.';

      while (point < 0) {
        tmp[i++] = '0';
        point++;
      }

      mpz_get_str(&(tmp[i]), 10, number);

      if (tmp[i + len + negative - 1] == '\0') {
        len--;
        total--;
      }

      if (negative)
        memmove(&(tmp[i]), &(tmp[i + 1]), len);

      tmp[total] = '\0';
    } else {
      // absolute length + negative sign + point sign
      total = len + negative + 1;
      point = point + negative;
      tmp   = (char*) emalloc((total + 1) * sizeof(char));

      mpz_get_str(tmp, 10, number);

      if (tmp[len + negative - 1] == '\0') {
        len--;
        total--;
      }

      memmove(&(tmp[point + 1]), &(tmp[point]), len - point);

      tmp[point] = '.';
      tmp[total] = '\0';
    }
  } else {
    // absolute length + negative sign + exponent modifier and sign
    total = len + negative + 2;
    // (optional) point sign
    if (len > 1)
      total++;

    // exponent value
    int exponent      = point - 1;
    int exponent_size = (int) ceil(log10(abs(exponent) + 2)) + 1;

    total = total + exponent_size;
    tmp   = (char*) emalloc((total + 1) * sizeof(char));

    mpz_get_str(tmp, 10, number);

    if (tmp[len + negative - 1] == '\0') {
      len--;
      total--;
    }

    int i = 1;
    if (negative)
      i++;

    memmove(&(tmp[i + 1]), &(tmp[i]), len - i);
    tmp[i] = '.';
    tmp[len + i++] = 'E';

    snprintf(&(tmp[len + i]), exponent_size, "%+d", exponent);

    tmp[total] = '\0';
  }

  *out     = tmp;
  *out_len = total;
}

void
import_twos_complement(cass_byte_t* data, cass_size_t size, mpz_t* number)
{
  int i;

  mpz_import(*number, size, 1, sizeof(cass_byte_t), 1, 0, data);

  // negative value
  if ((data[0] & 0x80) == 0x80) {
    // invert bits
    mpz_com(*number, *number);
    // add one
    mpz_add_ui(*number, *number, 1);
    // negate the value
    mpz_neg(*number, *number);
  }
}

cass_byte_t*
export_twos_complement(mpz_t number, cass_size_t* size)
{
  // negative, do two's complement
  if (mpz_sgn(number) == -1) {
    // invert bits
    mpz_com(number, number);
    // add one
    mpz_add_ui(number, number, 1);
  }

  return (cass_byte_t*) mpz_export(NULL, size, 1, sizeof(cass_byte_t), 1, 0, number);
}
