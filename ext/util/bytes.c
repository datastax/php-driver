#include "php_cassandra.h"
#include <stdlib.h>
#include "util/bytes.h"

void
php_cassandra_bytes_to_hex(const char* bin, int size, char** out, int* out_len)
{
  char hex_str[] = "0123456789abcdef";
  int  i;

  *out_len = (size - 1) * 2 + 2;
  *out = (char *) emalloc(sizeof(char) * ((size - 1) * 2 + 3));
  (*out)[0] = '0';
  (*out)[1] = 'x';
  (*out)[(size - 1) * 2 + 2] = '\0';

  for (i = 0; i < (size - 1); i++) {
    (*out)[i * 2 + 2] = hex_str[(bin[i] >> 4) & 0x0F];
    (*out)[i * 2 + 3] = hex_str[(bin[i]     ) & 0x0F];
  }
}
