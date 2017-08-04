/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "php_driver.h"
#include "php_driver_types.h"
#include <errno.h>
#include <stdlib.h>
#include <gmp.h>
#include <math.h>
#include "util/math.h"

#ifdef _WIN32
#  if defined(DISABLE_MSVC_STDINT) || _MSC_VER <= 1700
#    define strtoull _strtoui64
       float strtof(const char *str, char **endptr) {
         return (float) strtod(str, endptr);
       }
#  endif
#endif

extern zend_class_entry *php_driver_invalid_argument_exception_ce;

static int
prepare_string_conversion(char *in, int *pos, int *negative)
{
  int base = 0;
  int point = 0;

  /* Advance the pointer; ignore sign */
  if (in[point] == '+') {
    point++;
  } else if (in[point] == '-') {
    point++;
    if (negative) {
      *negative = 1;
    }
  }

  /* Handle special case for binary e.g. "0b0100" */
  if (in[point] == '0' && in[point + 1] == 'b') {
    base = 2;
    point += 2; /* Skip over "0b" */
  }

  /* Assign the position */
  if (pos) {
    *pos = point;
  }

  return base;
}

int
php_driver_parse_float(char *in, int in_len, cass_float_t *number TSRMLS_DC)
{
  char *end;
  errno = 0;

  *number = (cass_float_t) strtof(in, &end);

  if (errno == ERANGE) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too small or too big for float: '%s'", in);
    return 0;
  }

  if (errno || end == in) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid float value: '%s'", in);
    return 0;
  }

  if (end != &in[in_len]) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid characters were found in value: '%s'", in);
    return 0;
  }

  return 1;
}

int
php_driver_parse_double(char* in, int in_len, cass_double_t* number TSRMLS_DC)
{
  char* end;
  errno = 0;

  *number = (cass_double_t) strtod(in, &end);

  if (errno == ERANGE) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC, "Value is too small or too big for double: '%s'", in);
    return 0;
  }

  if (errno || end == in) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid double value: '%s'", in);
    return 0;
  }

  if (end != &in[in_len]) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid characters were found in value: '%s'", in);
    return 0;
  }

  return 1;
}

int
php_driver_parse_int(char* in, int in_len, cass_int32_t* number TSRMLS_DC)
{
  char* end = NULL;
  int pos = 0;
  int negative = 0;
  cass_uint32_t temp = 0;
  int base = 0;

  base = prepare_string_conversion(in, &pos, &negative);
  errno = 0;
  temp = (cass_uint32_t) strtoul(in + pos, &end, base);

  if (negative) {
    if (temp > (cass_uint32_t) INT_MAX + 1) {
      errno = ERANGE;
      *number = INT_MIN;
    } else if (temp == (cass_uint32_t) INT_MAX + 1) {
      *number = INT_MIN;
    } else {
      *number = -((cass_int32_t) temp);
    }
  } else {
    if (temp > (cass_uint32_t) INT_MAX) {
      errno = ERANGE;
      *number = INT_MAX;
    } else {
      *number = temp;
    }
  }

  if (errno == ERANGE) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
      "value must be between %d and %d, %s given", INT_MIN, INT_MAX, in);
    return 0;
  }

  if (errno || end == &in[pos]) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid integer value: '%s'", in);
    return 0;
  }

  if (end != &in[in_len]) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid characters were found in value: '%s'", in);
    return 0;
  }

  return 1;
}

int
php_driver_parse_bigint(char *in, int in_len, cass_int64_t *number TSRMLS_DC)
{
  char* end = NULL;
  int pos = 0;
  int negative = 0;
  cass_uint64_t temp = 0;
  int base = 0;

  base = prepare_string_conversion(in, &pos, &negative);
  errno = 0;
  temp = (cass_uint64_t) strtoull(in + pos, &end, base);

  if (negative) {
    if (temp > (cass_uint64_t) INT64_MAX + 1) {
      errno = ERANGE;
      *number = INT64_MIN;
    } else if (temp == (cass_uint64_t) INT64_MAX + 1) {
      *number = INT64_MIN;
    } else {
      *number = -((cass_int64_t) temp);
    }
  } else {
    if (temp > (cass_uint64_t) INT64_MAX) {
      errno = ERANGE;
      *number = INT64_MAX;
    } else {
      *number = temp;
    }
  }

  if (errno == ERANGE) {
    zend_throw_exception_ex(php_driver_range_exception_ce, 0 TSRMLS_CC,
      "value must be between " LL_FORMAT " and " LL_FORMAT ", %s given", INT64_MIN, INT64_MAX, in);
    return 0;
  }

  if (errno || end == &in[pos]) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid integer value: '%s'", in);
    return 0;
  }

  if (end != &in[in_len]) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid characters were found in value: '%s'", in);
    return 0;
  }

  return 1;
}

int
php_driver_parse_varint(char *in, int in_len, mpz_t *number TSRMLS_DC)
{
  int pos = 0;
  int negative = 0;
  int base = 0;

  base = prepare_string_conversion(in, &pos, &negative);

  if (mpz_set_str(*number, &in[pos], base) == -1) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid integer value: '%s'", in);
    return 0;
  }

  if (negative)
    mpz_neg(*number, *number);

  return 1;
}

int
php_driver_parse_decimal(char *in, int in_len, mpz_t *number, long *scale TSRMLS_DC)
{
  /*  start is the index into the char array where the significand starts */
  int start = 0;
  /*
   *  point is the index into the char array where the exponent starts
   *  (or, if there is no exponent, this is equal to end)
   */
  int point = 0;
  /*
   * dot is the index into the char array where the decimal point is
   * found, or -1 if there is no decimal point
   */
  int dot = -1;
  /*
   * out will be storing the string representation of the integer part
   * of the decimal value
   */
  char* out = (char*) ecalloc((in_len + 1), sizeof(char));
  /*  holds length of the formatted integer number */
  int out_len = 0;

  int maybe_octal = 0;

  /*
   * The following examples show what these variables mean.  Note that
   * point and dot don't yet have the correct values, they will be
   * properly assigned in a loop later on in this method.
   *
   * Example 1
   *
   *        +  1  0  2  .  4  6  9
   * __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
   *
   * offset = 2, in_len = 8, start = 3, dot = 6, point = end = 10
   *
   * Example 2
   *
   *        +  2  3  4  .  6  1  3  E  -  1
   * __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
   *
   * offset = 2, in_len = 11, start = 3, dot = 6, point = 10, end = 13
   *
   * Example 3
   *
   *        -  1  2  3  4  5  e  7
   * __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
   *
   * offset = 2, in_len = 8, start = 3, dot = -1, point = 8, end = 10
   */

  /* Determine the sign of the number. */
  int negative = 0;
  if (in[start] == '+') {
    start++;
    point++;
  } else if (in[start] == '-') {
    start++;
    point++;
    negative = 1;
  }

  maybe_octal = (in[point] == '0');

  /* Hex or binary */
  if (maybe_octal && (in[point + 1] == 'b' || in[point + 1] == 'x')) {
    *scale = 0;
    return php_driver_parse_varint(in, in_len, number TSRMLS_CC);
  }

  /*
   * Check each character looking for the decimal point and the
   * start of the exponent.
   */
  while (point < in_len) {
    char c = in[point];

    if (c == '.') {
      /* If dot != -1 then we've seen more than one decimal point. */
      if (dot != -1) {
        zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Multiple '.' (dots) in the number '%s'", in);
        return 0;
      }

      dot = point;
    }
    /* Break when we reach the start of the exponent. */
    else if (c == 'e' || c == 'E')
      break;
    /*
     * Throw an exception if the character was not a decimal or an
     * exponent and is not a hexadecimal digit.
     */
    else if (!isxdigit(c)) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Unrecognized character '%c' at position %d", c, point);
      return 0;
    }

    point++;
  }

  /* Octal number */
  if (maybe_octal && dot == -1) {
    *scale = 0;
    return php_driver_parse_varint(in, in_len, number TSRMLS_CC);
  }

  /* Prepend a negative sign if necessary. */
  if (negative)
    out[0] = '-';

  if (dot != -1) {
    /*
     * If there was a decimal we must combine the two parts that
     * contain only digits and we must set the scale properly.
     */
    memcpy(&out[negative], &in[start], dot - start);
    memcpy(&out[negative + dot - start], &in[dot + 1], point - dot);

    out_len = point - start + negative - 1;
    *scale = point - 1 - dot;
  } else {
    /*
     * If there was no decimal then the unscaled value is just the number
     * formed from all the digits and the scale is zero.
     */
    memcpy(&out[negative], &in[start], point - start);
    out_len = point - start + negative;
    *scale = 0;
  }

  if (out_len == 0) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "No digits seen in value: '%s'", in);
    return 0;
  }

  if (mpz_set_str(*number, out, 10) == -1) {
    zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Unable to extract integer part of decimal value: '%s', %s", in, out);
    efree(out);
    return 0;
  }

  efree(out);

  /*
   * Now parse exponent.
   * If point < end that means we broke out of the previous loop when we
   * saw an 'e' or an 'E'.
   */
  if (point < in_len) {
    int diff;

    point++;
    /* Ignore a '+' sign. */
    if (in[point] == '+')
      point++;

    /*
     * Throw an exception if there were no digits found after the 'e'
     * or 'E'.
     */
    if (point >= in_len) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "No exponent following e or E in value: '%s'", in);
      return 0;
    }

    if (!sscanf(&in[point], "%d", &diff)) {
      zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Malformed exponent in value: '%s'", in);
      return 0;
    }

    *scale = *scale - diff;
  }

  return 1;
}

void
php_driver_format_integer(mpz_t number, char **out, int *out_len)
{
  /* Adding 2 ensures enough space for the null-terminator and negative sign */
  *out = (char*) emalloc(mpz_sizeinbase(number, 10) + 2);
  mpz_get_str(*out, 10, number);
  *out_len = strlen(*out);
}

void
php_driver_format_decimal(mpz_t number, long scale, char **out, int *out_len)
{
  char *tmp = NULL;
  size_t total = 0;
  size_t len   = mpz_sizeinbase(number, 10);
  int negative = 0;
  int point = -1;

  if (scale == 0) {
    php_driver_format_integer(number, out, out_len);
    return;
  }

  if (mpz_sgn(number) < 0)
    negative = 1;

  // Ultimately, we want to return a string representation of this decimal. So allocate
  // a buffer that could hold this decimal in the worst possible conservative case.

  // absolute length + negative sign + point sign + scale (in case we end up with a number with leading 0s) +
  // exponent modifier and sign.
  tmp = (char*) emalloc(len + negative + 1 + scale + 2);
  mpz_get_str(tmp, 10, number);

  // Update len to be the true length of the string representation of |number|. mpz_sizeinbase
  // can return a higher result than the actual length.
  // NOTE: the length of the string includes the negative sign (if present); account for that.
  len  = strlen(tmp) - negative;

  point = len - scale;

  // We only support numbers with scale >= 0.
  assert(scale >= 0);

  if ((point - 1) >= -6) {
    if (point <= 0) {
      // e.g. -0.002 and 0.002
      int shift_start = negative;

      // current position
      int i = 0;

      // Move the numeric part (skip leading minus if needed) of tmp right by enough bytes to make room for
      // 0.0000 (as many leading zeroes as necessary).
      memmove(&(tmp[shift_start + 2 - point]), &(tmp[shift_start]), len);

      // This is a (possibly negative) number with a 0 integer part.
      if (negative)
        tmp[i++] = '-';

      tmp[i++] = '0';
      tmp[i++] = '.';

      // Add leading zeroes.
      while (point < 0) {
        tmp[i++] = '0';
        point++;
      }

      total = i + len;
      tmp[total] = '\0';
    } else {
      // e.g. 1.2, -1.2
      /* absolute length + negative sign + point sign */
      total = len + negative + 1;

      // Insert the decimal point at the right location in the string.

      // point is the index at which to insert the decimal point, but it assumes we have a positive
      // number. Move it to the right if we have a negative number.
      point += negative;

      memmove(&(tmp[point + 1]), &(tmp[point]), total - point);

      tmp[point] = '.';
      tmp[total] = '\0';
    }
  } else {
    // Very small positive or negative number that we want to express in scientific notation:
    // 0.000000004, -0.000000004

    int exponent = -1;
    int exponent_size = -1;

    // Calculate the exponent value and its size.
    exponent      = point - 1;
    exponent_size = (int) ceil(log10(abs(exponent) + 2)) + 1;

    // If we only have one significant digit, we want to produce a string like
    // 1E-9. If we have more significant digits, then 1.123E-9.

    if (len == 1) {
      // Simple case; tmp is already leading with our number as we want it. Append E(exp) to it
      // and we're done.
      sprintf(&(tmp[1 + negative]), "E%+d", exponent);
      total = 1 + negative + 1 + exponent_size;
    } else {
      // We have a more complex number. Insert a decimal point after the first digit.
      point = negative ? 2 : 1;
      memmove(&(tmp[point + 1]), &(tmp[point]), len-1);
      tmp[point] = '.';

      // Now append the exponent to the end and we're done.
      sprintf(&(tmp[point + len]), "E%+d", exponent);
      total = point + len + 1 + exponent_size;
    }
  }

  *out     = tmp;
  *out_len = total;
}

void
import_twos_complement(cass_byte_t *data, size_t size, mpz_t *number)
{
  mpz_import(*number, size, 1, sizeof(cass_byte_t), 1, 0, data);

  /* negative value */
  if ((data[0] & 0x80) == 0x80) {
    /* mpz_import() imports the two's complement value as an unsigned integer
     * so this needs to subtract 2^(8 * num_bytes) to get the negative value.
     */
    mpz_t temp;
    mpz_init(temp);
    mpz_set_ui(temp, 1);
    mpz_mul_2exp(temp, temp, 8 * size);
    mpz_sub(*number, *number, temp);
    mpz_clear(temp);
  }
}

cass_byte_t*
export_twos_complement(mpz_t number, size_t *size)
{
  cass_byte_t *bytes;

  if (mpz_sgn(number) == 0) {
    /* mpz_export() returns NULL for 0 */
    bytes = (cass_byte_t*) malloc(sizeof(cass_byte_t));
    *bytes = 0;
    *size = 1;
  } else if (mpz_sgn(number) == -1) {
    /*  mpz_export() ignores sign and only exports abs(number)
     *  so this needs to convert the number to the two's complement
     *  unsigned value.
     */
    size_t n;
    mpz_t temp;

    /* determine the number of bytes used in the two's complement
     * respresentation.
     */
    n = mpz_sizeinbase(number, 2) / 8 + 1;

    /* there's a special case for -2^(8 * n) numbers e.g. -128 (1000 0000) and
     * -32768 (100 0000 0000 0000), etc. that can be handled by n - 1 bytes in
     *  two's complement.
     */
    if (mpz_scan1(number, 0) == (8 * (n - 1)) - 1) {
      n--;
    }

    /* Add 2^(8 * num_bytes) to get the unsigned value e.g.
     * -1   + 2^8 = 255
     * -128 + 2^8 = 128
     * -129 + 2^16 = 65407
     * -32768 + 2^16 = 32768
     *  ...
     */
    mpz_init(temp);
    mpz_set_ui(temp, 1);
    mpz_mul_2exp(temp, temp, 8 * n);
    mpz_add(temp, number, temp);
    bytes = (cass_byte_t*) mpz_export(NULL, size, 1, sizeof(cass_byte_t), 1, 0, temp);
    mpz_clear(temp);
  } else {
    /* mpz_export() always returns a unsigned number and can have
     * values where the most significate bit is set. A 0 byte prevents
     * these from being interpreted as a negative value in two's complement
     */

    /* round to the nearest byte and add space for a leading 0 byte */
    *size = (mpz_sizeinbase(number, 2) + 7) / 8 + 1;
    bytes = malloc(*size);
    bytes[0] = 0;
    mpz_export(bytes + 1, NULL, 1, sizeof(cass_byte_t), 1, 0, number);
  }

  return bytes;
}
