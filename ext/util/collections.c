#include "php_cassandra.h"
#include <stdlib.h>
#include "util/collections.h"
#include "util/bytes.h"
#include "util/math.h"

#define EXPECTING_VALUE(expected) \
{ \
  throw_invalid_argument(object, "argument", expected TSRMLS_CC); \
  return 0; \
}

#define INSTANCE_OF(cls) \
  (Z_TYPE_P(object) == IS_OBJECT && instanceof_function(Z_OBJCE_P(object), cls TSRMLS_CC))

#define CHECK_ERROR(rc) ASSERT_SUCCESS_BLOCK(rc, result = 0;)

int
php_cassandra_validate_object(zval *object, CassValueType type TSRMLS_DC)
{
  if (Z_TYPE_P(object) == IS_NULL)
    return 1;

  switch (type) {
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
  case CASS_VALUE_TYPE_TEXT:
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
    if (PHP5TO7_ZVAL_IS_BOOL_P(object)) {
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
  default:
    EXPECTING_VALUE("a simple Cassandra value");

    return 0;
  }
}

int
php_cassandra_hash_object(zval *object, CassValueType type, char **key, int *len TSRMLS_DC)
{
  char *string;
  int string_len;
  cassandra_blob *blob = NULL;
  cassandra_numeric *numeric = NULL;
  cassandra_timestamp *timestamp = NULL;
  cassandra_uuid *uuid = NULL;
  cassandra_inet *inet = NULL;

  if (Z_TYPE_P(object) == IS_NULL) {
    *key = estrdup("C:NULL");
    *len = strlen("C:NULL");
    return 1;
  }

  switch (type) {
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
  case CASS_VALUE_TYPE_TEXT:
    if (Z_TYPE_P(object) != IS_STRING) {
      EXPECTING_VALUE("a string");
    }

    *len = spprintf(key, 0, "C:STRING:%s", Z_STRVAL_P(object));
    return 1;
  case CASS_VALUE_TYPE_DOUBLE:
    if (Z_TYPE_P(object) != IS_DOUBLE) {
      EXPECTING_VALUE("a float");
    }

    *len = spprintf(key, 0, "C:FLOAT:%f", Z_DVAL_P(object));
    return 1;
  case CASS_VALUE_TYPE_INT:
    if (Z_TYPE_P(object) != IS_LONG) {
      EXPECTING_VALUE("an int");
    }

    *len = spprintf(key, 0, "C:INT:%ld", Z_LVAL_P(object));
    return 1;
  case CASS_VALUE_TYPE_BOOLEAN:
    if (!PHP5TO7_ZVAL_IS_BOOL_P(object)) {
      EXPECTING_VALUE("a boolean");
    }

#if PHP_MAJOR_VERSION >= 7
    *len = spprintf(key, 0, "C:BOOLEAN:%d", Z_TYPE_P(object) == IS_TRUE ? 1 : 0);
#else
    *len = spprintf(key, 0, "C:BOOLEAN:%d", Z_BVAL_P(object));
#endif
    return 1;
  case CASS_VALUE_TYPE_FLOAT:
    if (!INSTANCE_OF(cassandra_float_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Float");
    }

    numeric = PHP_CASSANDRA_GET_NUMERIC(object);
    *len = spprintf(key, 0, "C:FLOAT:%.*F", (int) EG(precision), numeric->float_value);

    return 1;
  case CASS_VALUE_TYPE_COUNTER:
  case CASS_VALUE_TYPE_BIGINT:
    if (!INSTANCE_OF(cassandra_bigint_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Bigint");
    }

    numeric = PHP_CASSANDRA_GET_NUMERIC(object);
#ifdef WIN32
    *len = spprintf(key, 0, "C:BIGINT:%I64d", (long long int) numeric->bigint_value);
#else
    *len = spprintf(key, 0, "C:BIGINT:%lld", (long long int) numeric->bigint_value);
#endif
    return 1;
  case CASS_VALUE_TYPE_BLOB:
    if (!INSTANCE_OF(cassandra_blob_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Blob");
    }

    blob = PHP_CASSANDRA_GET_BLOB(object);
    php_cassandra_bytes_to_hex((const char*) blob->data, blob->size, &string, &string_len);

    *len = spprintf(key, 0, "C:BLOB:%s", string);
    efree(string);
    return 1;
  case CASS_VALUE_TYPE_DECIMAL:
    if (!INSTANCE_OF(cassandra_decimal_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Decimal");
    }

    numeric = PHP_CASSANDRA_GET_NUMERIC(object);
    php_cassandra_format_integer(numeric->decimal_value, &string, &string_len);

    *len = spprintf(key, 0, "C:DECIMAL:%s:%ld", string, numeric->decimal_scale);
    efree(string);
    return 1;
  case CASS_VALUE_TYPE_TIMESTAMP:
    if (!INSTANCE_OF(cassandra_timestamp_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Timestamp");
    }

    timestamp = PHP_CASSANDRA_GET_TIMESTAMP(object);
#ifdef WIN32
    *len = spprintf(key, 0, "C:TIMESTAMP:%I64d", (long long int) timestamp->timestamp);
#else
    *len = spprintf(key, 0, "C:TIMESTAMP:%lld", (long long int) timestamp->timestamp);
#endif

    return 1;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    if (!INSTANCE_OF(cassandra_uuid_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Uuid");
    }

    uuid = PHP_CASSANDRA_GET_UUID(object);
#ifdef WIN32
    *len = spprintf(key, 0, "C:UUID:%I64d:%I64d", (long long int) uuid->uuid.time_and_version, (long long int) uuid->uuid.clock_seq_and_node);
#else
    *len = spprintf(key, 0, "C:UUID:%lld:%lld", (long long int) uuid->uuid.time_and_version, (long long int) uuid->uuid.clock_seq_and_node);
#endif

    return 1;
  case CASS_VALUE_TYPE_VARINT:
    if (!INSTANCE_OF(cassandra_varint_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Varint");
    }

    numeric = PHP_CASSANDRA_GET_NUMERIC(object);
    php_cassandra_format_integer(numeric->varint_value, &string, &string_len);

    *len = spprintf(key, 0, "C:VARINT:%s", string);
    efree(string);
    return 1;
  case CASS_VALUE_TYPE_INET:
    if (!INSTANCE_OF(cassandra_inet_ce)) {
      EXPECTING_VALUE("an instance of Cassandra\\Inet");
    }

    inet = PHP_CASSANDRA_GET_INET(object);
    if (inet->inet.address_length > 4)
      *len = spprintf(key, 0, "C:INET:%x:%x:%x:%x:%x:%x:%x:%x",
        (inet->inet.address[0]  * 256 + inet->inet.address[1]),
        (inet->inet.address[2]  * 256 + inet->inet.address[3]),
        (inet->inet.address[4]  * 256 + inet->inet.address[5]),
        (inet->inet.address[6]  * 256 + inet->inet.address[7]),
        (inet->inet.address[8]  * 256 + inet->inet.address[9]),
        (inet->inet.address[10] * 256 + inet->inet.address[11]),
        (inet->inet.address[12] * 256 + inet->inet.address[13]),
        (inet->inet.address[14] * 256 + inet->inet.address[15])
      );
    else
      *len = spprintf(key, 0, "C:INET:%d.%d.%d.%d",
        inet->inet.address[0],
        inet->inet.address[1],
        inet->inet.address[2],
        inet->inet.address[3]
      );

    return 1;
  default:
    EXPECTING_VALUE("a simple Cassandra value");

    return 0;
  }

  return 0;
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

const char*
php_cassandra_type_name(CassValueType value_type)
{
  switch (value_type) {
  case CASS_VALUE_TYPE_TEXT:
    return "text";
  case CASS_VALUE_TYPE_ASCII:
    return "ascii";
  case CASS_VALUE_TYPE_VARCHAR:
    return "varchar";
  case CASS_VALUE_TYPE_BIGINT:
    return "bigint";
  case CASS_VALUE_TYPE_BLOB:
    return "blob";
  case CASS_VALUE_TYPE_BOOLEAN:
    return "boolean";
  case CASS_VALUE_TYPE_COUNTER:
    return "counter";
  case CASS_VALUE_TYPE_DECIMAL:
    return "decimal";
  case CASS_VALUE_TYPE_DOUBLE:
    return "double";
  case CASS_VALUE_TYPE_FLOAT:
    return "float";
  case CASS_VALUE_TYPE_INT:
    return "int";
  case CASS_VALUE_TYPE_TIMESTAMP:
    return "timestamp";
  case CASS_VALUE_TYPE_UUID:
    return "uuid";
  case CASS_VALUE_TYPE_VARINT:
    return "varint";
  case CASS_VALUE_TYPE_TIMEUUID:
    return "timeuuid";
  case CASS_VALUE_TYPE_INET:
    return "inet";
  default:
    return "unknown";
  }
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
    data = (cass_byte_t*) export_twos_complement(numeric->varint_value, &size);
    CHECK_ERROR(cass_collection_append_bytes(collection, data, size));
    free(data);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    numeric = PHP_CASSANDRA_GET_NUMERIC(value);
    data = (cass_byte_t*) export_twos_complement(numeric->decimal_value, &size);
    CHECK_ERROR(cass_collection_append_decimal(collection, data, size, numeric->decimal_scale));
    free(data);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = PHP_CASSANDRA_GET_INET(value);
    CHECK_ERROR(cass_collection_append_inet(collection, inet->inet));
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
  php5to7_zval *current;
  CassCollection *collection =
      cass_collection_new(CASS_COLLECTION_TYPE_SET, zend_hash_num_elements(&set->values));

  PHP5TO7_ZEND_HASH_FOREACH_VAL(&set->values, current) {
    if (!php_cassandra_collection_append(collection, PHP5TO7_ZVAL_MAYBE_DEREF(current), set->type TSRMLS_CC)) {
      result = 0;
      break;
    }
  } PHP5TO7_ZEND_HASH_FOREACH_END(&set->values);

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
  CassCollection *collection =
      cass_collection_new(CASS_COLLECTION_TYPE_LIST, zend_hash_num_elements(&coll->values));

  PHP5TO7_ZEND_HASH_FOREACH_VAL(&coll->values, current) {
    if (!php_cassandra_collection_append(collection, PHP5TO7_ZVAL_MAYBE_DEREF(current), coll->type TSRMLS_CC)) {
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
  HashPosition keys_pos;
  HashPosition values_pos;
  php5to7_zval *current;
  CassCollection *collection =
      cass_collection_new(CASS_COLLECTION_TYPE_MAP, zend_hash_num_elements(&map->keys));

  zend_hash_internal_pointer_reset_ex(&map->keys, &keys_pos);
  zend_hash_internal_pointer_reset_ex(&map->values, &values_pos);

  collection = cass_collection_new(CASS_COLLECTION_TYPE_MAP, zend_hash_num_elements(&map->keys));

  while (PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(&map->keys, current)) {
    if (php_cassandra_collection_append(collection, PHP5TO7_ZVAL_MAYBE_DEREF(current), map->key_type TSRMLS_CC)) {
      PHP5TO7_ZEND_HASH_GET_CURRENT_DATA(&map->values, current);
      if (!php_cassandra_collection_append(collection, PHP5TO7_ZVAL_MAYBE_DEREF(current), map->value_type TSRMLS_CC)) {
        result = 0;
        break;
      }
    } else {
      result = 0;
      break;
    }

    zend_hash_move_forward_ex(&map->keys, &keys_pos);
    zend_hash_move_forward_ex(&map->values, &values_pos);
  }

  if (result)
    *collection_ptr = collection;
  else
    cass_collection_free(collection);

  return result;
}
