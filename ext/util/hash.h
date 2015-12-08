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

#endif /* PHP_CASSANDRA_HASH_H */

