#ifndef PHP_CASSANDRA_HASH_H
#define PHP_CASSANDRA_HASH_H

#define uthash_malloc(sz) emalloc(sz)
#define uthash_free(ptr,sz) efree(ptr)

#define HASH_FUNCTION(key,keylen,num_bkts,hashv,bkt)                \
  hashv = php_cassandra_value_hash(*((zval**)key) TSRMLS_CC); \
  bkt = (hashv) & (num_bkts - 1U)

#define HASH_KEYCOMPARE(a, b, len) \
  php_cassandra_value_compare(*((zval**)a), *((zval**)b) TSRMLS_CC)

#undef HASH_ADD /* Previously defined in Zend/zend_hash.h */

#include "util/uthash.h"

struct cassandra_map_entry_ {
  zval* key;
  zval* value;
  UT_hash_handle hh;
};

struct cassandra_set_entry_ {
  zval* value;
  UT_hash_handle hh;
};

#define PHP_CASSANDRA_COMPARE(a, b) ((a) < (b) ? -1 : (a) > (b))

unsigned php_cassandra_value_hash(zval* zvalue TSRMLS_DC);
int php_cassandra_value_compare(zval* zvalue1, zval* zvalue2 TSRMLS_DC);

unsigned php_cassandra_mpz_hash(unsigned seed, mpz_t n);

static inline unsigned php_cassandra_bigint_hash(cass_int64_t value) {
  return (unsigned)(value ^ (value >> 32));
}

static inline unsigned php_cassandra_combine_hash(unsigned seed, unsigned  hashv) {
  return seed ^ (hashv + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

#endif /* PHP_CASSANDRA_HASH_H */
