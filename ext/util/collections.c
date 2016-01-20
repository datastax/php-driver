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

int
php_cassandra_validate_object(zval *object, zval *ztype TSRMLS_DC)
{
  cassandra_type *type;

  if (Z_TYPE_P(object) == IS_NULL)
    return 1;

  type = PHP_CASSANDRA_GET_TYPE(ztype);

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
    if (!PHP5TO7_ZVAL_IS_BOOL_P(object)) {
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
      cassandra_map *map = PHP_CASSANDRA_GET_MAP(object);
      cassandra_type *map_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map->type));
      if (php_cassandra_type_compare(map_type, type TSRMLS_CC) != 0) {
        return 0;
      }
    }

    return 1;
 case CASS_VALUE_TYPE_SET:
    if (!INSTANCE_OF(cassandra_set_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Set");
    } else {
      cassandra_set *set = PHP_CASSANDRA_GET_SET(object);
      cassandra_type *set_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));
      if (php_cassandra_type_compare(set_type, type TSRMLS_CC) != 0) {
        return 0;
      }
    }

    return 1;
 case CASS_VALUE_TYPE_LIST:
    if (!INSTANCE_OF(cassandra_collection_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Collection");
    } else {
      cassandra_collection *collection = PHP_CASSANDRA_GET_COLLECTION(object);
      cassandra_type *collection_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(collection->type));
      if (php_cassandra_type_compare(collection_type, type TSRMLS_CC) != 0) {
        return 0;
      }
    }

    return 1;
 case CASS_VALUE_TYPE_UDT:
    if (!INSTANCE_OF(cassandra_user_type_value_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\UserTypeValue");
    } else {
      cassandra_user_type_value *user_type_value = PHP_CASSANDRA_GET_USER_TYPE_VALUE(object);
      cassandra_type *user_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(user_type_value->type));
      if (php_cassandra_type_compare(user_type, type TSRMLS_CC) != 0) {
        return 0;
      }
    }

    return 1;
  default:
    EXPECTING_VALUE("a simple Cassandra value");

    return 0;
  }
}

int
php_cassandra_value_type(char *type, CassValueType *value_type TSRMLS_DC)
{
  if (strcmp("ascii", type) == 0) {
    *value_type = CASS_VALUE_TYPE_ASCII;
  } else if (strcmp("bigint", type) == 0) {
    *value_type = CASS_VALUE_TYPE_BIGINT;
  } else if (strcmp("blob", type) == 0) {
    *value_type = CASS_VALUE_TYPE_BLOB;
  } else if (strcmp("boolean", type) == 0) {
    *value_type = CASS_VALUE_TYPE_BOOLEAN;
  } else if (strcmp("counter", type) == 0) {
    *value_type = CASS_VALUE_TYPE_COUNTER;
  } else if (strcmp("decimal", type) == 0) {
    *value_type = CASS_VALUE_TYPE_DECIMAL;
  } else if (strcmp("double", type) == 0) {
    *value_type = CASS_VALUE_TYPE_DOUBLE;
  } else if (strcmp("float", type) == 0) {
    *value_type = CASS_VALUE_TYPE_FLOAT;
  } else if (strcmp("int", type) == 0) {
    *value_type = CASS_VALUE_TYPE_INT;
  } else if (strcmp("text", type) == 0) {
    *value_type = CASS_VALUE_TYPE_TEXT;
  } else if (strcmp("timestamp", type) == 0) {
    *value_type = CASS_VALUE_TYPE_TIMESTAMP;
  } else if (strcmp("uuid", type) == 0) {
    *value_type = CASS_VALUE_TYPE_UUID;
  } else if (strcmp("varchar", type) == 0) {
    *value_type = CASS_VALUE_TYPE_VARCHAR;
  } else if (strcmp("varint", type) == 0) {
    *value_type = CASS_VALUE_TYPE_VARINT;
  } else if (strcmp("timeuuid", type) == 0) {
    *value_type = CASS_VALUE_TYPE_TIMEUUID;
  } else if (strcmp("inet", type) == 0) {
    *value_type = CASS_VALUE_TYPE_INET;
  } else {
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
      "Unsupported type '%s'", type);
    return 0;
  }

  return 1;
}

static int
php_cassandra_collection_append(CassCollection *collection, zval *value, CassValueType type TSRMLS_DC)
{
  int result = 1;
  cassandra_blob       *blob;
  cassandra_numeric    *numeric;
  cassandra_timestamp  *timestamp;
  cassandra_uuid       *uuid;
  cassandra_inet       *inet;
  size_t                size;
  cass_byte_t          *data;
  cassandra_collection *coll;
  cassandra_map        *map;
  cassandra_set        *set;
  cassandra_user_type_value *user_type_value;
  CassCollection       *sub_collection;
  CassUserType         *sub_ut;

  switch (type) {
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
    CHECK_ERROR(cass_collection_append_string_n(collection, Z_STRVAL_P(value), Z_STRLEN_P(value)));
    break;
  case CASS_VALUE_TYPE_BIGINT:
  case CASS_VALUE_TYPE_COUNTER:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_ERROR(cass_collection_append_int64(collection, numeric->bigint_value));
    break;
  case CASS_VALUE_TYPE_BLOB:
    blob = PHP_CASSANDRA_GET_BLOB(value);
    CHECK_ERROR(cass_collection_append_bytes(collection, blob->data, blob->size));
    break;
  case CASS_VALUE_TYPE_BOOLEAN:
#if PHP_MAJOR_VERSION >= 7
    CHECK_ERROR(cass_collection_append_bool(collection, Z_TYPE_P(value) == IS_TRUE ? cass_true : cass_false));
#else
    CHECK_ERROR(cass_collection_append_bool(collection, Z_BVAL_P(value)));
#endif
    break;
  case CASS_VALUE_TYPE_DOUBLE:
    CHECK_ERROR(cass_collection_append_double(collection, Z_DVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_FLOAT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_ERROR(cass_collection_append_float(collection, numeric->float_value));
    break;
  case CASS_VALUE_TYPE_INT:
    CHECK_ERROR(cass_collection_append_int32(collection, Z_LVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    timestamp = PHP_CASSANDRA_GET_TIMESTAMP(value);
    CHECK_ERROR(cass_collection_append_int64(collection, timestamp->timestamp));
    break;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    uuid = PHP_CASSANDRA_GET_UUID(value);
    CHECK_ERROR(cass_collection_append_uuid(collection, uuid->uuid));
    break;
  case CASS_VALUE_TYPE_VARINT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    data = (cass_byte_t *) export_twos_complement(numeric->varint_value, &size);
    CHECK_ERROR(cass_collection_append_bytes(collection, data, size));
    free(data);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    data = (cass_byte_t *) export_twos_complement(numeric->decimal_value, &size);
    CHECK_ERROR(cass_collection_append_decimal(collection, data, size, numeric->decimal_scale));
    free(data);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = PHP_CASSANDRA_GET_INET(value);
    CHECK_ERROR(cass_collection_append_inet(collection, inet->inet));
    break;
  case CASS_VALUE_TYPE_LIST:
    coll = PHP_CASSANDRA_GET_COLLECTION(value);
    if (!php_cassandra_collection_from_collection(coll, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_collection_append_collection(collection, sub_collection));
    break;
  case CASS_VALUE_TYPE_MAP:
    map = PHP_CASSANDRA_GET_MAP(value);
    if (!php_cassandra_collection_from_map(map, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_collection_append_collection(collection, sub_collection));
    break;
  case CASS_VALUE_TYPE_SET:
    set = PHP_CASSANDRA_GET_SET(value);
    if (!php_cassandra_collection_from_set(set, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_collection_append_collection(collection, sub_collection));
    break;
  case CASS_VALUE_TYPE_UDT:
    user_type_value = PHP_CASSANDRA_GET_USER_TYPE_VALUE(value);
    if (!php_cassandra_user_type_from_user_type_value(user_type_value, &sub_ut TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_collection_append_user_type(collection, sub_ut));
    break;
  default:
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC, "Unsupported collection type");
    return 0;
  }

  return result;
}

static int
php_cassandra_user_type_set(CassUserType *ut,
                            const char* name, zval *value,
                            CassValueType type TSRMLS_DC)
{
  int result = 1;
  cassandra_blob       *blob;
  cassandra_numeric    *numeric;
  cassandra_timestamp  *timestamp;
  cassandra_uuid       *uuid;
  cassandra_inet       *inet;
  size_t                size;
  cass_byte_t          *data;
  cassandra_collection *coll;
  cassandra_map        *map;
  cassandra_set        *set;
  cassandra_user_type_value *user_type_value;
  CassCollection       *sub_collection;
  CassUserType         *sub_ut;

  switch (type) {
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
    CHECK_ERROR(cass_user_type_set_string_by_name(ut, name, Z_STRVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_BIGINT:
  case CASS_VALUE_TYPE_COUNTER:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_ERROR(cass_user_type_set_int64_by_name(ut, name, numeric->bigint_value));
    break;
  case CASS_VALUE_TYPE_BLOB:
    blob = PHP_CASSANDRA_GET_BLOB(value);
    CHECK_ERROR(cass_user_type_set_bytes_by_name(ut, name, blob->data, blob->size));
    break;
  case CASS_VALUE_TYPE_BOOLEAN:
#if PHP_MAJOR_VERSION >= 7
    CHECK_ERROR(cass_user_type_set_bool_by_name(ut, name, Z_TYPE_P(value) == IS_TRUE ? cass_true : cass_false));
#else
    CHECK_ERROR(cass_user_type_set_bool_by_name(ut, name, Z_BVAL_P(value)));
#endif
    break;
  case CASS_VALUE_TYPE_DOUBLE:
    CHECK_ERROR(cass_user_type_set_double_by_name(ut, name, Z_DVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_FLOAT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    CHECK_ERROR(cass_user_type_set_float_by_name(ut, name, numeric->float_value));
    break;
  case CASS_VALUE_TYPE_INT:
    CHECK_ERROR(cass_user_type_set_int32_by_name(ut, name, Z_LVAL_P(value)));
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    timestamp = PHP_CASSANDRA_GET_TIMESTAMP(value);
    CHECK_ERROR(cass_user_type_set_int64_by_name(ut, name, timestamp->timestamp));
    break;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    uuid = PHP_CASSANDRA_GET_UUID(value);
    CHECK_ERROR(cass_user_type_set_uuid_by_name(ut, name, uuid->uuid));
    break;
  case CASS_VALUE_TYPE_VARINT:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    data = (cass_byte_t *) export_twos_complement(numeric->varint_value, &size);
    CHECK_ERROR(cass_user_type_set_bytes_by_name(ut, name, data, size));
    free(data);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    data = (cass_byte_t *) export_twos_complement(numeric->decimal_value, &size);
    CHECK_ERROR(cass_user_type_set_decimal_by_name(ut, name, data, size, numeric->decimal_scale));
    free(data);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = PHP_CASSANDRA_GET_INET(value);
    CHECK_ERROR(cass_user_type_set_inet_by_name(ut, name, inet->inet));
    break;
  case CASS_VALUE_TYPE_LIST:
    coll = PHP_CASSANDRA_GET_COLLECTION(value);
    if (!php_cassandra_collection_from_collection(coll, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_user_type_set_collection_by_name(ut, name, sub_collection));
    break;
  case CASS_VALUE_TYPE_MAP:
    map = PHP_CASSANDRA_GET_MAP(value);
    if (!php_cassandra_collection_from_map(map, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_user_type_set_collection_by_name(ut, name, sub_collection));
    break;
  case CASS_VALUE_TYPE_SET:
    set = PHP_CASSANDRA_GET_SET(value);
    if (!php_cassandra_collection_from_set(set, &sub_collection TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_user_type_set_collection_by_name(ut, name, sub_collection));
    break;
  case CASS_VALUE_TYPE_UDT:
    user_type_value = PHP_CASSANDRA_GET_USER_TYPE_VALUE(value);
    if (!php_cassandra_user_type_from_user_type_value(user_type_value, &sub_ut TSRMLS_CC))
      return 0;
    CHECK_ERROR(cass_user_type_set_user_type_by_name(ut, name, sub_ut));
    break;
  default:
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC, "Unsupported collection type");
    return 0;
  }

  return result;
}

int
php_cassandra_collection_from_set(cassandra_set *set, CassCollection **collection_ptr TSRMLS_DC)
{
  int result = 1;
  CassCollection *collection = NULL;
  cassandra_type *type;
  cassandra_type *value_type;
  cassandra_set_entry *curr, *temp;

  collection = cass_collection_new(CASS_COLLECTION_TYPE_SET, HASH_COUNT(set->entries));

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(set->type));
  value_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));

  HASH_ITER(hh, set->entries, curr, temp) {
    if (!php_cassandra_collection_append(collection,
                                         PHP5TO7_ZVAL_MAYBE_P(curr->value),
                                         value_type->type TSRMLS_CC)) {
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
php_cassandra_collection_from_collection(cassandra_collection *coll, CassCollection **collection_ptr TSRMLS_DC)
{
  int result = 1;
  php5to7_zval *current;
  cassandra_type *type;
  cassandra_type *value_type;
  CassCollection *collection =
      cass_collection_new(CASS_COLLECTION_TYPE_LIST, zend_hash_num_elements(&coll->values));

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(coll->type));
  value_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));

  PHP5TO7_ZEND_HASH_FOREACH_VAL(&coll->values, current) {
    if (!php_cassandra_collection_append(collection, PHP5TO7_ZVAL_MAYBE_DEREF(current), value_type->type TSRMLS_CC)) {
      result = 0;
      break;
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(&coll->values);

  if (result)
    *collection_ptr = collection;
  else
    cass_collection_free(collection);

  return result;
}

int
php_cassandra_collection_from_map(cassandra_map *map, CassCollection **collection_ptr TSRMLS_DC)
{
  int result = 1;
  CassCollection *collection = NULL;
  cassandra_type *type;
  cassandra_type *key_type;
  cassandra_type *value_type;
  cassandra_map_entry *curr, *temp;

  collection = cass_collection_new(CASS_COLLECTION_TYPE_MAP, HASH_COUNT(map->entries));

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(map->type));
  value_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->value_type));
  key_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(type->key_type));

  HASH_ITER(hh, map->entries, curr, temp) {
    if (!php_cassandra_collection_append(collection,
                                         PHP5TO7_ZVAL_MAYBE_P(curr->key),
                                         key_type->type TSRMLS_CC)) {
      result = 0;
      break;
    }
    if (!php_cassandra_collection_append(collection,
                                         PHP5TO7_ZVAL_MAYBE_P(curr->value),
                                         value_type->type TSRMLS_CC)) {
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
php_cassandra_user_type_from_user_type_value(cassandra_user_type_value *user_type_value,
                                             CassUserType **output TSRMLS_DC)
{
  int result = 1;
  char *name;
  php5to7_zval *current;
  cassandra_type *type;
  CassUserType *ut;

  type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_P(user_type_value->type));
  ut = cass_user_type_new_from_data_type(type->data_type);

  PHP5TO7_ZEND_HASH_FOREACH_STR_KEY_VAL(&user_type_value->values, name, current) {
    php5to7_zval *zsub_type;
    cassandra_type *sub_type;
    if (!PHP5TO7_ZEND_HASH_FIND(&type->types, name, strlen(name) + 1, zsub_type) ||
        !php_cassandra_validate_object(PHP5TO7_ZVAL_MAYBE_DEREF(current),
                                       PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type) TSRMLS_CC)) {
      result = 0;
      break;
    }
    sub_type = PHP_CASSANDRA_GET_TYPE(PHP5TO7_ZVAL_MAYBE_DEREF(zsub_type));
    if (!php_cassandra_user_type_set(ut,
                                     name,
                                     PHP5TO7_ZVAL_MAYBE_DEREF(current),
                                     sub_type->type TSRMLS_CC)) {
      result = 0;
      break;
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(&user_type_value->values);

  if (result)
    *output = ut;
  else
    cass_user_type_free(ut);

  return result;
}

