#include <php.h>
#include "../php_cassandra.h"
#include <stdlib.h>
#include "util/bytes.h"

void
php_cassandra_bytes_to_hex(const char* bin, int len, char** out, int* out_len)
{
  char hex_str[] = "0123456789abcdef";
  int  i;

  *out_len = len * 2 + 2;
  *out = (char *) emalloc(sizeof(char) * (len * 2 + 3));
  (*out)[0] = '0';
  (*out)[1] = 'x';
  (*out)[len * 2 + 2] = '\0';

  for (i = 0; i < len; i++) {
    (*out)[i * 2 + 2] = hex_str[(bin[i] >> 4) & 0x0F];
    (*out)[i * 2 + 3] = hex_str[(bin[i]     ) & 0x0F];
  }
}
