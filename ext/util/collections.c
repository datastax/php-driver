#include "php_cassandra.h"
#include <stdlib.h>
#include "util/bytes.h"
#include "util/collections.h"
#include "util/hash.h"
#include "util/math.h"
#include "util/types.h"

#define EXPECTING_VALUE(expected) \
{ \
  throw_invalid_argument(object, "argument", expected TSRMLS_CC); \
  return 0; \
}

#define INSTANCE_OF(cls) \
  (Z_TYPE_P(object) == IS_OBJECT && instanceof_function(Z_OBJCE_P(object), cls TSRMLS_CC))

#define CHECK_ERROR(rc) ASSERT_SUCCESS_BLOCK(rc, result = 0;)

static inline cass_int64_t
double_to_bits(cass_double_t value) {
  cass_int64_t bits;
  if (zend_isnan(value)) return 0x7ff8000000000000LL; /* A canonical NaN value */
  memcpy(&bits, &value, sizeof(cass_int64_t));
  return bits;
}

static inline cass_int32_t
float_to_bits(cass_float_t value) {
  cass_int32_t bits;
  if (zend_isnan(value)) return 0x7fc00000; /* A canonical NaN value */
  memcpy(&bits, &value, sizeof(cass_int32_t));
  return bits;
}

static inline unsigned
combine_hash(unsigned seed, unsigned  hashv) {
  return seed ^ (hashv + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

static inline unsigned
blob_hash(cassandra_blob* value) {
  return zend_inline_hash_func((const char*)value->data, value->size);
}

static inline unsigned
bigint_hash(cass_int64_t value) {
  return (unsigned)(value ^ (value >> 32));
}

static inline unsigned
double_hash(cass_double_t value) {
  return bigint_hash(double_to_bits(value));
}

static inline unsigned
float_hash(cass_float_t value) {
  return float_to_bits(value);
}

static inline unsigned
mpz_hash(unsigned seed, mpz_t n) {
  size_t i;
  size_t size = mpz_size(n);
  const mp_limb_t* limbs = mpz_limbs_read(n);
  unsigned hashv = seed;
#if GMP_LIMB_BITS == 32
    for (i = 0; i < size; ++i) {
      hashv = combine_hash(hashv, limbs[i]);
    }
#elif GMP_LIMB_BITS == 64
    for (i = 0; i < size; ++i) {
      hashv = combine_hash(hashv, bigint_hash(limbs[i]));
    }
#else
#error "Unexpected GMP limb bits size"
#endif
    return hashv;
}

static inline unsigned
decimal_hash(cassandra_decimal* value) {
  return mpz_hash((unsigned)value->scale, value->value);
}

static inline unsigned
uuid_hash(cassandra_uuid* value) {
  return combine_hash(bigint_hash(value->uuid.time_and_version),
                      bigint_hash(value->uuid.clock_seq_and_node));
}

static inline unsigned
varint_hash(cassandra_varint* value) {
  return mpz_hash(0, value->value);
}

static inline unsigned
inet_hash(cassandra_inet* value) {
  return zend_inline_hash_func((const char*)value->inet.address,
                               value->inet.address_length);
}

static inline unsigned
collection_hash(cassandra_collection* collection TSRMLS_DC) {
  HashPointer ptr;
  zval** current;

  unsigned hashv = 0;
  if (!collection->dirty) return collection->hashv;

  zend_hash_get_pointer(&collection->values, &ptr);
  zend_hash_internal_pointer_reset(&collection->values);

  while (zend_hash_get_current_data(&collection->values, (void**) &current) == SUCCESS) {
    hashv = combine_hash(hashv, php_cassandra_value_hash(*current TSRMLS_CC));
    zend_hash_move_forward(&collection->values);
  }

  zend_hash_set_pointer(&collection->values, &ptr);

  collection->hashv = hashv;
  collection->dirty = 0;

  return hashv;
}

static inline unsigned
map_hash(cassandra_map* map TSRMLS_DC) {
  cassandra_map_entry* curr, * temp;
  unsigned hashv = 0;

  if (!map->dirty) return map->hashv;

  HASH_ITER(hh, map->entries, curr, temp) {
    hashv = combine_hash(hashv, php_cassandra_value_hash(curr->key TSRMLS_CC));
    hashv = combine_hash(hashv, php_cassandra_value_hash(curr->value TSRMLS_CC));
  }

  map->hashv = hashv;
  map->dirty = 0;

  return hashv;
}

static inline unsigned
set_hash(cassandra_set* set TSRMLS_DC) {
  cassandra_set_entry* curr, * temp;

  unsigned hashv = 0;
  if (!set->dirty) return set->hashv;

  HASH_ITER(hh, set->entries, curr, temp) {
    hashv = combine_hash(hashv, php_cassandra_value_hash(curr->value TSRMLS_CC));
  }

  set->hashv = hashv;
  set->dirty = 0;

  return hashv;
}

static unsigned
value_hash(cassandra_value* value TSRMLS_DC) {
  cassandra_type* type
      = (cassandra_type*) zend_object_store_get_object(value->type TSRMLS_CC);

  switch(type->type) {
    case CASS_VALUE_TYPE_BIGINT:
    case CASS_VALUE_TYPE_COUNTER:
      return bigint_hash(((cassandra_bigint*)value)->value);

    case CASS_VALUE_TYPE_BLOB:
      return blob_hash((cassandra_blob*)value);

    case CASS_VALUE_TYPE_DECIMAL:
      return decimal_hash((cassandra_decimal*)value);

    case CASS_VALUE_TYPE_FLOAT:
      return float_hash(((cassandra_float*)value)->value);

    case CASS_VALUE_TYPE_TIMESTAMP:
      return bigint_hash(((cassandra_timestamp*)value)->timestamp);

    case CASS_VALUE_TYPE_UUID:
    case CASS_VALUE_TYPE_TIMEUUID:
      return uuid_hash((cassandra_uuid*)value);

    case CASS_VALUE_TYPE_VARINT:
      return varint_hash((cassandra_varint*)value);

    case CASS_VALUE_TYPE_INET:
      return inet_hash((cassandra_inet*)value);

    case CASS_VALUE_TYPE_LIST:
      return collection_hash((cassandra_collection*)value TSRMLS_CC);

    case CASS_VALUE_TYPE_MAP:
      return map_hash((cassandra_map*)value TSRMLS_CC);

    case CASS_VALUE_TYPE_SET:
      return set_hash((cassandra_set*)value TSRMLS_CC);

    default:
      break;
  }

  return 0;
}

unsigned
php_cassandra_value_hash(zval* zvalue TSRMLS_DC) {
  cassandra_value* value;

  switch (Z_TYPE_P(zvalue)) {
  case IS_LONG:
#if SIZEOF_LONG == 4
    return Z_LVAL_P(zvalue);
#elif SIZEOF_LONG == 8
    return bigint_hash(Z_LVAL_P(zvalue));
#else
#error "Unexpected sizeof(long)"
#endif

  case IS_DOUBLE:
    return double_hash(Z_DVAL_P(zvalue));

  case IS_BOOL:
    return Z_BVAL_P(zvalue);

  case IS_STRING:
    return zend_inline_hash_func(Z_STRVAL_P(zvalue), Z_STRLEN_P(zvalue));

  case IS_OBJECT:
    value = (cassandra_value*) zend_object_store_get_object(zvalue TSRMLS_CC);
    return value_hash(value TSRMLS_CC);

  default:
    break;
  }

  return 0;
}

#define COMPARE(a, b) ((a) < (b) ? -1 : (a) > (b))

static inline int
blob_compare(cassandra_blob* value1, cassandra_blob* value2) {
  if (value1->size != value2->size)
    return value1->size < value2->size ? -1 : 1;
  return memcmp(value1->data, value1->data, value1->size);
}

static inline int
double_compare(cass_double_t d1, cass_double_t d2) {
  cass_int64_t bits1, bits2;
  if (d1 < d2) return -1;
  if (d1 > d2) return  1;
  bits1 = double_to_bits(d1);
  bits2 = double_to_bits(d2);
  /* Handle NaNs and negative and positive 0.0 */
  return COMPARE(bits1, bits2);
}

static inline int
float_compare(cassandra_float* value1, cassandra_float* value2) {
  cass_int64_t bits1, bits2;
  cass_double_t d1 = value1->value;
  cass_double_t d2 = value2->value;
  if (d1 < d2) return -1;
  if (d1 > d2) return  1;
  bits1 = double_to_bits(d1);
  bits2 = double_to_bits(d2);
  /* Handle NaNs and negative and positive 0.0 */
  return COMPARE(bits1, bits2);
}

static inline int
decimal_compare(cassandra_decimal* value1, cassandra_decimal* value2) {
  if (value1->scale != value2->scale) {
    return value1->scale < value2->scale ? -1 : 1;
  }
  return mpz_cmp(value1->value, value2->value);
}

static inline int
uuid_compare_(cassandra_uuid* value1, cassandra_uuid* value2) {
  if (value1->uuid.time_and_version != value2->uuid.time_and_version)
    return value1->uuid.time_and_version < value2->uuid.time_and_version ? -1 : 1;
  if (value1->uuid.clock_seq_and_node != value2->uuid.clock_seq_and_node)
    return value1->uuid.clock_seq_and_node < value2->uuid.clock_seq_and_node ? -1 : 1;
  return 0;
}

static inline int
varint_compare(cassandra_varint* value1, cassandra_varint* value2) {
  return mpz_cmp(value1->value, value2->value);
}

static inline int
inet_compare(cassandra_inet* value1, cassandra_inet* value2) {
  if (value1->inet.address_length != value2->inet.address_length) {
   return value1->inet.address_length < value2->inet.address_length ? -1 : 1;
  }
  return memcmp(value1->inet.address, value2->inet.address, value1->inet.address_length);
}

static inline int
collection_compare(cassandra_collection* collection1,
                   cassandra_collection* collection2 TSRMLS_DC) {
  HashPointer ptr1;
  zval** current1;
  HashPointer ptr2;
  zval** current2;

  if (zend_hash_num_elements(&collection1->values) != zend_hash_num_elements(&collection2->values)) {
    return zend_hash_num_elements(&collection1->values) < zend_hash_num_elements(&collection2->values) ? -1 : 1;
  }

  zend_hash_get_pointer(&collection1->values, &ptr1);
  zend_hash_internal_pointer_reset(&collection1->values);

  zend_hash_get_pointer(&collection2->values, &ptr2);
  zend_hash_internal_pointer_reset(&collection2->values);

  while (zend_hash_get_current_data(&collection1->values, (void**) &current1) == SUCCESS &&
         zend_hash_get_current_data(&collection2->values, (void**) &current2) == SUCCESS) {
    int r = php_cassandra_value_compare(*current1, *current2 TSRMLS_CC);
    if (r != 0) return r;
  }

  zend_hash_set_pointer(&collection1->values, &ptr1);
  zend_hash_set_pointer(&collection2->values, &ptr2);

  return 0;
}

static inline int
map_compare(cassandra_map* map1, cassandra_map* map2 TSRMLS_DC) {
  cassandra_map_entry* iter1;
  cassandra_map_entry* iter2;

  if (HASH_COUNT(map1->entries) != HASH_COUNT(map1->entries)) {
   return HASH_COUNT(map1->entries) < HASH_COUNT(map1->entries) ? -1 : 1;
  }

  iter1 = map1->entries;
  iter2 = map2->entries;
  while (iter1 && iter2) {
    int r;

    r = php_cassandra_value_compare(iter1->key, iter2->key TSRMLS_CC);
    if (r != 0) return r;

    r = php_cassandra_value_compare(iter1->value, iter2->value TSRMLS_CC);
    if (r != 0) return r;

    iter1 = (cassandra_map_entry*)iter1->hh.next;
    iter2 = (cassandra_map_entry*)iter2->hh.next;
  }

  return 0;
}

static inline int
set_compare(cassandra_set* set1, cassandra_set* set2 TSRMLS_DC) {
  cassandra_set_entry* iter1;
  cassandra_set_entry* iter2;

  if (HASH_COUNT(set1->entries) != HASH_COUNT(set1->entries)) {
   return HASH_COUNT(set1->entries) < HASH_COUNT(set1->entries) ? -1 : 1;
  }

  iter1 = set1->entries;
  iter2 = set2->entries;
  while (iter1 && iter2) {
    int r = php_cassandra_value_compare(iter1->value, iter2->value TSRMLS_CC);
    if (r != 0) return r;
  }

  return 0;
}

static int
value_compare(cassandra_value* value1, cassandra_value* value2 TSRMLS_DC) {
  cassandra_type* type1;
  cassandra_type* type2;

  int result;

  if (value1 == value2) return 0;

  type1 = (cassandra_type*) zend_object_store_get_object(value1->type TSRMLS_CC);
  type2 = (cassandra_type*) zend_object_store_get_object(value2->type TSRMLS_CC);

  result = php_cassandra_type_compare(type1, type2 TSRMLS_CC);

  if (result != 0) return result;

  switch(type1->type) {
    case CASS_VALUE_TYPE_BIGINT:
    case CASS_VALUE_TYPE_COUNTER:
      return COMPARE(((cassandra_bigint*)value1)->value, ((cassandra_bigint*)value2)->value);

    case CASS_VALUE_TYPE_BLOB:
      return blob_compare((cassandra_blob*)value1, (cassandra_blob*)value2);

    case CASS_VALUE_TYPE_DECIMAL:
      return decimal_compare((cassandra_decimal*)value1, (cassandra_decimal*)value2);

    case CASS_VALUE_TYPE_FLOAT:
      return float_compare((cassandra_float*)value1, (cassandra_float*)value2);

    case CASS_VALUE_TYPE_TIMESTAMP:
      return COMPARE(((cassandra_timestamp*)value1)->timestamp, ((cassandra_timestamp*)value2)->timestamp);

    case CASS_VALUE_TYPE_UUID:
    case CASS_VALUE_TYPE_TIMEUUID:
      return uuid_compare_((cassandra_uuid*)value1, (cassandra_uuid*)value2);

    case CASS_VALUE_TYPE_VARINT:
      return varint_compare((cassandra_varint*)value1, (cassandra_varint*)value2);

    case CASS_VALUE_TYPE_INET:
      return inet_compare((cassandra_inet*)value1, (cassandra_inet*)value2);

    case CASS_VALUE_TYPE_LIST:
      return collection_compare((cassandra_collection*)value1, (cassandra_collection*)value2 TSRMLS_CC);

    case CASS_VALUE_TYPE_MAP:
      return map_compare((cassandra_map*)value1, (cassandra_map*)value2 TSRMLS_CC);

    case CASS_VALUE_TYPE_SET:
      return set_compare((cassandra_set*)value1, (cassandra_set*)value2 TSRMLS_CC);

    default:
      break;
  }

  return 0;
}

int
php_cassandra_value_compare(zval* zvalue1, zval* zvalue2 TSRMLS_DC) {
  cassandra_value* value1, * value2;
  if (zvalue1 == zvalue2) return 0;

  if (Z_TYPE_P(zvalue1) != Z_TYPE_P(zvalue2)) {
    return Z_TYPE_P(zvalue1)  < Z_TYPE_P(zvalue2) ? -1 : 1;
  }

  switch (Z_TYPE_P(zvalue1)) {
  case IS_LONG:
    return COMPARE(Z_LVAL_P(zvalue1), Z_LVAL_P(zvalue2));

  case IS_DOUBLE:
    return double_compare(Z_DVAL_P(zvalue1), Z_DVAL_P(zvalue2));

  case IS_BOOL:
    return COMPARE(Z_DVAL_P(zvalue1), Z_DVAL_P(zvalue2));

  case IS_STRING:
    return zend_binary_zval_strcmp(zvalue1, zvalue2);

  case IS_OBJECT:
    value1 = (cassandra_value*) zend_object_store_get_object(zvalue1 TSRMLS_CC);
    value2 = (cassandra_value*) zend_object_store_get_object(zvalue2 TSRMLS_CC);
    return value_compare(value1, value2 TSRMLS_CC);

  default:
    break;
  }

  return 1;
}

#undef COMPARE

int
php_cassandra_validate_object(zval* object, zval* ztype TSRMLS_DC)
{
  if (Z_TYPE_P(object) == IS_NULL)
    return 1;

  cassandra_type* type = (cassandra_type*) zend_object_store_get_object(ztype TSRMLS_CC);

  switch (type->type) {
  case CASS_VALUE_TYPE_VARCHAR:
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_ASCII:
    if (Z_TYPE_P(object) != IS_STRING) {
      EXPECTING_VALUE("a string");
    }

    return 1;
  case CASS_VALUE_TYPE_DOUBLE:
    if (Z_TYPE_P(object) != IS_DOUBLE) {
      EXPECTING_VALUE("a float");
    }

    return 1;
  case CASS_VALUE_TYPE_INT:
    if (Z_TYPE_P(object) != IS_LONG) {
      EXPECTING_VALUE("an int");
    }

    return 1;
  case CASS_VALUE_TYPE_BOOLEAN:
    if (Z_TYPE_P(object) != IS_BOOL) {
      EXPECTING_VALUE("a boolean");
    }

    return 1;
  case CASS_VALUE_TYPE_FLOAT:
    if (!INSTANCE_OF(cassandra_float_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Float");
    }

    return 1;
  case CASS_VALUE_TYPE_COUNTER:
  case CASS_VALUE_TYPE_BIGINT:
    if (!INSTANCE_OF(cassandra_bigint_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Bigint");
    }

    return 1;
  case CASS_VALUE_TYPE_BLOB:
    if (!INSTANCE_OF(cassandra_blob_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Blob");
    }

    return 1;
  case CASS_VALUE_TYPE_DECIMAL:
    if (!INSTANCE_OF(cassandra_decimal_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Decimal");
    }

    return 1;
  case CASS_VALUE_TYPE_TIMESTAMP:
    if (!INSTANCE_OF(cassandra_timestamp_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Timestamp");
    }

    return 1;
  case CASS_VALUE_TYPE_UUID:
    if (!INSTANCE_OF(cassandra_uuid_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Uuid");
    }

    return 1;
  case CASS_VALUE_TYPE_VARINT:
    if (!INSTANCE_OF(cassandra_varint_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Varint");
    }

    return 1;
  case CASS_VALUE_TYPE_TIMEUUID:
    if (!INSTANCE_OF(cassandra_timeuuid_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Timeuuid");
    }

    return 1;
  case CASS_VALUE_TYPE_INET:
    if (!INSTANCE_OF(cassandra_inet_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Inet");
    }

    return 1;
 case CASS_VALUE_TYPE_MAP:
    if (!INSTANCE_OF(cassandra_map_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Map");
    } else {
      cassandra_map* map = (cassandra_map*) zend_object_store_get_object(object TSRMLS_CC);
      cassandra_type* map_type = (cassandra_type*) zend_object_store_get_object(map->type TSRMLS_CC);
      if (php_cassandra_type_compare(map_type, type TSRMLS_CC) != 0) {
        return 0;
      }
    }

    return 1;
 case CASS_VALUE_TYPE_SET:
    if (!INSTANCE_OF(cassandra_set_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Set");
    } else {
      cassandra_set* set = (cassandra_set*) zend_object_store_get_object(object TSRMLS_CC);
      cassandra_type* set_type = (cassandra_type*) zend_object_store_get_object(set->type TSRMLS_CC);
      if (php_cassandra_type_compare(set_type, type TSRMLS_CC) != 0) {
        return 0;
      }
    }

    return 1;
 case CASS_VALUE_TYPE_LIST:
    if (!INSTANCE_OF(cassandra_collection_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Collection");
    } else {
      cassandra_collection* collection = (cassandra_collection*) zend_object_store_get_object(object TSRMLS_CC);
      cassandra_type* collection_type = (cassandra_type*) zend_object_store_get_object(collection->type TSRMLS_CC);
      if (php_cassandra_type_compare(collection_type, type TSRMLS_CC) != 0) {
        return 0;
      }
    }

    return 1;
  default:
    EXPECTING_VALUE("a simple Cassandra value");

    return 0;
  }
}

static int
php_cassandra_collection_append(CassCollection* collection, zval* value, CassValueType type TSRMLS_DC)
{
  int result = 1;
  cassandra_float*      float_number;
  cassandra_bigint*     bigint;
  cassandra_blob*       blob;
  cassandra_timestamp*  timestamp;
  cassandra_uuid*       uuid;
  cassandra_varint*     varint;
  cassandra_decimal*    decimal;
  cassandra_inet*       inet;
  size_t                size;
  cass_byte_t*          data;

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
  cassandra_collection* coll;
  cassandra_map*        map;
  cassandra_set*        set;
  CassCollection*       sub_collection;
#endif

  switch (type) {
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
    CHECK_ERROR(cass_collection_append_string_n(collection, Z_STRVAL_P(value), Z_STRLEN_P(value)));
    break;
  case CASS_VALUE_TYPE_BIGINT:
  case CASS_VALUE_TYPE_COUNTER:
    bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_int64(collection, bigint->value));
    break;
  case CASS_VALUE_TYPE_BLOB:
    blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_bytes(collection, blob->data, blob->size));
    break;
  case CASS_VALUE_TYPE_BOOLEAN:
    CHECK_ERROR(cass_collection_append_bool(collection, Z_BVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_DOUBLE:
    CHECK_ERROR(cass_collection_append_double(collection, Z_DVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_FLOAT:
    float_number = (cassandra_float*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_float(collection, float_number->value));
    break;
  case CASS_VALUE_TYPE_INT:
    CHECK_ERROR(cass_collection_append_int32(collection, Z_LVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    timestamp = (cassandra_timestamp*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_int64(collection, timestamp->timestamp));
    break;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    uuid = (cassandra_uuid*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_uuid(collection, uuid->uuid));
    break;
  case CASS_VALUE_TYPE_VARINT:
    varint = (cassandra_varint*) zend_object_store_get_object(value TSRMLS_CC);
    data = (cass_byte_t*) export_twos_complement(varint->value, &size);
    CHECK_ERROR(cass_collection_append_bytes(collection, data, size));
    free(data);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
    data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
    CHECK_ERROR(cass_collection_append_decimal(collection, data, size, decimal->scale));
    free(data);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_inet(collection, inet->inet));
    break;
#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
  case CASS_VALUE_TYPE_LIST:
    coll = (cassandra_collection*) zend_object_store_get_object(value TSRMLS_CC);
    if (!php_cassandra_collection_from_collection(coll, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_collection_append_collection(collection, sub_collection));
    break;
  case CASS_VALUE_TYPE_MAP:
    map = (cassandra_map*) zend_object_store_get_object(value TSRMLS_CC);
    if (!php_cassandra_collection_from_map(map, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_collection_append_collection(collection, sub_collection));
    break;
  case CASS_VALUE_TYPE_SET:
    set = (cassandra_set*) zend_object_store_get_object(value TSRMLS_CC);
    if (!php_cassandra_collection_from_set(set, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_collection_append_collection(collection, sub_collection));
    break;
#endif
  default:
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC, "Unsupported collection type");
    return 0;
  }

  return result;
}

int
php_cassandra_collection_from_set(cassandra_set* set, CassCollection** collection_ptr TSRMLS_DC)
{
  int result = 1;
  CassCollection* collection = NULL;
  cassandra_type_set* type;
  cassandra_type* value_type;
  cassandra_set_entry* curr, * temp;

  collection = cass_collection_new(CASS_COLLECTION_TYPE_MAP, HASH_COUNT(set->entries));

  type = (cassandra_type_set*) zend_object_store_get_object(set->type TSRMLS_CC);
  value_type = (cassandra_type*) zend_object_store_get_object(type->value_type TSRMLS_CC);

  HASH_ITER(hh, set->entries, curr, temp) {
    if (!php_cassandra_collection_append(collection, curr->value, value_type->type TSRMLS_CC)) {
      result = 0;
      break;
    }
  }

  if (result)
    *collection_ptr = collection;
  else
    cass_collection_free(collection);

  return result;
}

int
php_cassandra_collection_from_collection(cassandra_collection* coll, CassCollection** collection_ptr TSRMLS_DC)
{
  int result = 1;
  HashPointer ptr;
  zval** current;
  CassCollection* collection = NULL;
  cassandra_type_collection* type;
  cassandra_type* value_type;

  zend_hash_get_pointer(&coll->values, &ptr);
  zend_hash_internal_pointer_reset(&coll->values);

  collection = cass_collection_new(CASS_COLLECTION_TYPE_LIST, zend_hash_num_elements(&coll->values));

  type = (cassandra_type_collection*) zend_object_store_get_object(coll->type TSRMLS_CC);
  value_type = (cassandra_type*) zend_object_store_get_object(type->value_type TSRMLS_CC);

  while (zend_hash_get_current_data(&coll->values, (void**) &current) == SUCCESS) {
    if (!php_cassandra_collection_append(collection, *current, value_type->type TSRMLS_CC)) {
      result = 0;
      break;
    }
    zend_hash_move_forward(&coll->values);
  }

  zend_hash_set_pointer(&coll->values, &ptr);

  if (result)
    *collection_ptr = collection;
  else
    cass_collection_free(collection);

  return result;
}

int
php_cassandra_collection_from_map(cassandra_map* map, CassCollection** collection_ptr TSRMLS_DC)
{
  int result = 1;
  CassCollection* collection = NULL;
  cassandra_type_map* type;
  cassandra_type* key_type;
  cassandra_type* value_type;
  cassandra_map_entry* curr, * temp;

  collection = cass_collection_new(CASS_COLLECTION_TYPE_MAP, HASH_COUNT(map->entries));

  type = (cassandra_type_map*) zend_object_store_get_object(map->type TSRMLS_CC);
  key_type = (cassandra_type*) zend_object_store_get_object(type->key_type TSRMLS_CC);
  value_type = (cassandra_type*) zend_object_store_get_object(type->value_type TSRMLS_CC);

  HASH_ITER(hh, map->entries, curr, temp) {
    if (!php_cassandra_collection_append(collection, curr->key, key_type->type TSRMLS_CC)) {
      result = 0;
      break;
    }
    if (!php_cassandra_collection_append(collection, curr->value, value_type->type TSRMLS_CC)) {
      result = 0;
      break;
    }
  }

  if (result)
    *collection_ptr = collection;
  else
    cass_collection_free(collection);

  return result;
}
