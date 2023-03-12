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
#pragma once

#include "inline.h"
#include <php_driver.h>

#define uthash_malloc(sz) emalloc(sz)
#define uthash_free(ptr, sz) efree(ptr)

#define HASH_FUNCTION(key, keylen, num_bkts, hashv, bkt)                                                               \
    hashv = php_driver_value_hash((zval *)key);                                                                        \
    bkt = (hashv) & (num_bkts - 1U)
#define HASH_KEYCOMPARE(a, b, len) php_driver_value_compare((zval *)a, (zval *)b)

#undef HASH_ADD /* Previously defined in Zend/zend_hash.h */

#include "uthash.h"

#define HASH_FIND_ZVAL(head, zvptr, out) HASH_FIND(hh, head, zvptr, 0, out)

#define HASH_ADD_ZVAL(head, fieldname, add) HASH_ADD_KEYPTR(hh, head, PHP5TO7_ZVAL_MAYBE_P(((add)->fieldname)), 0, add)

struct php_driver_map_entry_
{
    php5to7_zval key;
    php5to7_zval value;
    UT_hash_handle hh;
};

struct php_driver_set_entry_
{
    php5to7_zval value;
    UT_hash_handle hh;
};

#define PHP_DRIVER_COMPARE(a, b) ((a) < (b) ? -1 : (a) > (b))

uint32_t php_driver_value_hash(zval *zvalue);
int32_t php_driver_value_compare(zval *zvalue1, zval *zvalue2);
int32_t php_driver_data_compare(Bucket *a, Bucket *b);
uint32_t php_driver_mpz_hash(unsigned seed, mpz_t n);

static PHP_DRIVER_ALWAYS_INLINE uint32_t php_driver_bigint_hash(cass_int64_t value)
{
    return (uint32_t)(value ^ (value >> 32));
}

static PHP_DRIVER_ALWAYS_INLINE uint32_t php_driver_combine_hash(unsigned seed, unsigned hashv)
{
    return seed ^ (hashv + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}
