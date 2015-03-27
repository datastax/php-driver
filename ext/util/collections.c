#include "../php_cassandra.h"
#include <stdlib.h>
#include "util/collections.h"
#include "util/bytes.h"
#include "util/math.h"

#define EXPECTING_VALUE(expected) \
  ({ \
    if (Z_TYPE_P(object) == IS_OBJECT) { \
      Z_OBJ_HANDLER_P(object, get_class_name)(object, (const char**) &class_name, &class_name_len, 0 TSRMLS_CC); \
      if (class_name) { \
        zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, \
          "Expected " expected ", an instance of %s given", class_name); \
        efree(class_name); \
      } else { \
        zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, \
          "Expected " expected ", an instance of Unknown Class given"); \
      } \
    } else { \
      zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, \
        "Expected " expected ", %Z given", object); \
    } \
    return 0; \
  })

#define INSTANCE_OF(cls) \
  (Z_TYPE_P(object) == IS_OBJECT && instanceof_function(Z_OBJCE_P(object), cls TSRMLS_CC))

#define CHECK_ERROR(rc) \
  ({ \
    if (rc != CASS_OK) { \
      zend_throw_exception_ex(cassandra_ce_RuntimeException, 0 TSRMLS_CC, \
        "%s", cass_error_desc(rc)); \
      result = 0; \
    } \
  })

extern zend_class_entry* cassandra_ce_RuntimeException;
extern zend_class_entry* cassandra_ce_InvalidArgumentException;
extern zend_class_entry* cassandra_ce_Bigint;
extern zend_class_entry* cassandra_ce_Blob;
extern zend_class_entry* cassandra_ce_Decimal;
extern zend_class_entry* cassandra_ce_Float;
extern zend_class_entry* cassandra_ce_Inet;
extern zend_class_entry* cassandra_ce_Timestamp;
extern zend_class_entry* cassandra_ce_Timeuuid;
extern zend_class_entry* cassandra_ce_Uuid;
extern zend_class_entry* cassandra_ce_Varint;

int
php_cassandra_validate_object(zval* object, CassValueType type TSRMLS_DC)
{
  char* class_name = NULL;
  zend_uint class_name_len;

  if (Z_TYPE_P(object) == IS_NULL)
    return 1;

  switch (type) {
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
  case CASS_VALUE_TYPE_TEXT:
    if (Z_TYPE_P(object) != IS_STRING)
      EXPECTING_VALUE("a string");

    return 1;
  case CASS_VALUE_TYPE_DOUBLE:
    if (Z_TYPE_P(object) != IS_DOUBLE)
      EXPECTING_VALUE("a float");

    return 1;
  case CASS_VALUE_TYPE_INT:
    if (Z_TYPE_P(object) != IS_LONG)
      EXPECTING_VALUE("an int");

    return 1;
  case CASS_VALUE_TYPE_BOOLEAN:
    if (Z_TYPE_P(object) != IS_BOOL)
      EXPECTING_VALUE("a boolean");

    return 1;
  case CASS_VALUE_TYPE_FLOAT:
    if (!INSTANCE_OF(cassandra_ce_Float))
      EXPECTING_VALUE("an instance of Cassandra\\Float");

    return 1;
  case CASS_VALUE_TYPE_COUNTER:
  case CASS_VALUE_TYPE_BIGINT:
    if (!INSTANCE_OF(cassandra_ce_Bigint))
      EXPECTING_VALUE("an instance of Cassandra\\Bigint");

    return 1;
  case CASS_VALUE_TYPE_BLOB:
    if (!INSTANCE_OF(cassandra_ce_Blob))
      EXPECTING_VALUE("an instance of Cassandra\\Blob");

    return 1;
  case CASS_VALUE_TYPE_DECIMAL:
    if (!INSTANCE_OF(cassandra_ce_Decimal))
      EXPECTING_VALUE("an instance of Cassandra\\Decimal");

    return 1;
  case CASS_VALUE_TYPE_TIMESTAMP:
    if (!INSTANCE_OF(cassandra_ce_Timestamp))
      EXPECTING_VALUE("an instance of Cassandra\\Timestamp");

    return 1;
  case CASS_VALUE_TYPE_UUID:
    if (!INSTANCE_OF(cassandra_ce_Uuid))
      EXPECTING_VALUE("an instance of Cassandra\\Uuid");

    return 1;
  case CASS_VALUE_TYPE_VARINT:
    if (!INSTANCE_OF(cassandra_ce_Varint))
      EXPECTING_VALUE("an instance of Cassandra\\Varint");

    return 1;
  case CASS_VALUE_TYPE_TIMEUUID:
    if (!INSTANCE_OF(cassandra_ce_Timeuuid))
      EXPECTING_VALUE("an instance of Cassandra\\Timeuuid");

    return 1;
  case CASS_VALUE_TYPE_INET:
    if (!INSTANCE_OF(cassandra_ce_Inet))
      EXPECTING_VALUE("an instance of Cassandra\\Inet");

    return 1;
  default:
    EXPECTING_VALUE("a simple Cassandra value");

    return 0;
  }
}

int
php_cassandra_hash_object(zval* object, CassValueType type, char** key, int* len TSRMLS_DC)
{
  zval* value;
  zval* scale;
  char* class_name = NULL;
  zend_uint class_name_len;
  char* string;
  int string_len;

  if (Z_TYPE_P(object) == IS_NULL) {
    *key = strdup("C:NULL");
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
    if (Z_TYPE_P(object) != IS_BOOL) {
      EXPECTING_VALUE("a boolean");
    }

    *len = spprintf(key, 0, "C:BOOLEAN:%d", Z_BVAL_P(object));
    return 1;
  case CASS_VALUE_TYPE_FLOAT:
    if (!INSTANCE_OF(cassandra_ce_Float)) {
      EXPECTING_VALUE("an instance of Cassandra\\Float");
    }

    cassandra_float* float_number = (cassandra_float*) zend_object_store_get_object(object TSRMLS_CC);
    *len = spprintf(key, 0, "C:FLOAT:%.*F", (int) EG(precision), float_number->value);

    return 1;
  case CASS_VALUE_TYPE_COUNTER:
  case CASS_VALUE_TYPE_BIGINT:
    if (!INSTANCE_OF(cassandra_ce_Bigint)) {
      EXPECTING_VALUE("an instance of Cassandra\\Bigint");
    }

    cassandra_bigint* bigint = (cassandra_bigint*) zend_object_store_get_object(object TSRMLS_CC);
#ifdef WIN32
    *len = spprintf(key, 0, "C:BIGINT:%I64d", bigint->value);
#else
    *len = spprintf(key, 0, "C:BIGINT:%lld", bigint->value);
#endif
    return 1;
  case CASS_VALUE_TYPE_BLOB:
    if (!INSTANCE_OF(cassandra_ce_Blob)) {
      EXPECTING_VALUE("an instance of Cassandra\\Blob");
    }

    cassandra_blob* blob = (cassandra_blob*) zend_object_store_get_object(object TSRMLS_CC);
    php_cassandra_bytes_to_hex((const char*) blob->data, blob->size, &string, &string_len);

    *len = spprintf(key, 0, "C:BLOB:%s", string);
    efree(string);
    return 1;
  case CASS_VALUE_TYPE_DECIMAL:
    if (!INSTANCE_OF(cassandra_ce_Decimal)) {
      EXPECTING_VALUE("an instance of Cassandra\\Decimal");
    }

    cassandra_decimal* decimal = (cassandra_decimal*) zend_object_store_get_object(object TSRMLS_CC);
    php_cassandra_format_integer(decimal->value, &string, &string_len);

    *len = spprintf(key, 0, "C:DECIMAL:%s:%ld", string, decimal->scale);
    efree(string);
    return 1;
  case CASS_VALUE_TYPE_TIMESTAMP:
    if (!INSTANCE_OF(cassandra_ce_Timestamp)) {
      EXPECTING_VALUE("an instance of Cassandra\\Timestamp");
    }

    cassandra_timestamp* timestamp = (cassandra_timestamp*) zend_object_store_get_object(object TSRMLS_CC);

#ifdef WIN32
    *len = spprintf(key, 0, "C:TIMESTAMP:%I64d", timestamp->timestamp);
#else
    *len = spprintf(key, 0, "C:TIMESTAMP:%lld", timestamp->timestamp);
#endif
    return 1;
  case CASS_VALUE_TYPE_UUID:
  case CASS_VALUE_TYPE_TIMEUUID:
    if (!INSTANCE_OF(cassandra_ce_Uuid)) {
      EXPECTING_VALUE("an instance of Cassandra\\Uuid");
    }

    cassandra_uuid* uuid = (cassandra_uuid*) zend_object_store_get_object(object TSRMLS_CC);
#ifdef WIN32
    *len = spprintf(key, 0, "C:UUID:%I64d:%I64d", uuid->uuid.time_and_version, uuid->uuid.clock_seq_and_node);
#else
    *len = spprintf(key, 0, "C:UUID:%lld:%lld", uuid->uuid.time_and_version, uuid->uuid.clock_seq_and_node);
#endif
    return 1;
  case CASS_VALUE_TYPE_VARINT:
    if (!INSTANCE_OF(cassandra_ce_Varint)) {
      EXPECTING_VALUE("an instance of Cassandra\\Varint");
    }

    cassandra_varint* varint = (cassandra_varint*) zend_object_store_get_object(object TSRMLS_CC);
    php_cassandra_format_integer(varint->value, &string, &string_len);

    *len = spprintf(key, 0, "C:VARINT:%s", string);
    efree(string);
    return 1;
  case CASS_VALUE_TYPE_INET:
    if (!INSTANCE_OF(cassandra_ce_Inet)) {
      EXPECTING_VALUE("an instance of Cassandra\\Inet");
    }

    cassandra_inet* inet = (cassandra_inet*) zend_object_store_get_object(object TSRMLS_CC);
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
php_cassandra_value_type(char* type, CassValueType* value_type)
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
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC,
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
php_cassandra_collection_append(CassCollection* collection, zval* value, CassValueType type)
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
  cass_size_t           size;
  cass_byte_t*          data;

  switch (type) {
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_VARCHAR:
    CHECK_ERROR(cass_collection_append_string(collection, cass_string_init2(Z_STRVAL_P(value), Z_STRLEN_P(value))));
    break;
  case CASS_VALUE_TYPE_BIGINT:
  case CASS_VALUE_TYPE_COUNTER:
    bigint = (cassandra_bigint*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_int64(collection, bigint->value));
    break;
  case CASS_VALUE_TYPE_BLOB:
    blob = (cassandra_blob*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_bytes(collection, cass_bytes_init(blob->data, blob->size)));
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
    CHECK_ERROR(cass_collection_append_bytes(collection, cass_bytes_init(data, size)));
    free(data);
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    decimal = (cassandra_decimal*) zend_object_store_get_object(value TSRMLS_CC);
    data = (cass_byte_t*) export_twos_complement(decimal->value, &size);
    CHECK_ERROR(cass_collection_append_decimal(collection, cass_decimal_init(decimal->scale, cass_bytes_init(data, size))));
    free(data);
    break;
  case CASS_VALUE_TYPE_INET:
    inet = (cassandra_inet*) zend_object_store_get_object(value TSRMLS_CC);
    CHECK_ERROR(cass_collection_append_inet(collection, inet->inet));
    break;
  default:
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC, "Unsupported set type");
    return 0;
  }

  return result;
}

int
php_cassandra_collection_from_set(cassandra_set* set, CassCollection** collection_ptr)
{
  int result = 1;
  HashPointer ptr;
  zval** current;

  zend_hash_get_pointer(&set->values, &ptr);
  zend_hash_internal_pointer_reset(&set->values);

  CassCollection* collection = cass_collection_new(CASS_COLLECTION_TYPE_SET, zend_hash_num_elements(&set->values));

  while (zend_hash_get_current_data(&set->values, (void**) &current) == SUCCESS) {
    if (!php_cassandra_collection_append(collection, *current, set->type)) {
      result = 0;
      break;
    }
    zend_hash_move_forward(&set->values);
  }

  zend_hash_set_pointer(&set->values, &ptr);

  if (result)
    *collection_ptr = collection;
  else
    cass_collection_free(collection);

  return result;
}

int
php_cassandra_collection_from_collection(cassandra_collection* coll, CassCollection** collection_ptr)
{
  int result = 1;
  HashPointer ptr;
  zval** current;

  zend_hash_get_pointer(&coll->values, &ptr);
  zend_hash_internal_pointer_reset(&coll->values);

  CassCollection* collection = cass_collection_new(CASS_COLLECTION_TYPE_LIST, zend_hash_num_elements(&coll->values));

  while (zend_hash_get_current_data(&coll->values, (void**) &current) == SUCCESS) {
    if (!php_cassandra_collection_append(collection, *current, coll->type)) {
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
php_cassandra_collection_from_map(cassandra_map* map, CassCollection** collection_ptr)
{
  int result = 1;
  HashPointer keys_ptr, values_ptr;
  zval** current;

  zend_hash_get_pointer(&map->keys, &keys_ptr);
  zend_hash_internal_pointer_reset(&map->keys);

  zend_hash_get_pointer(&map->values, &values_ptr);
  zend_hash_internal_pointer_reset(&map->values);

  CassCollection* collection = cass_collection_new(CASS_COLLECTION_TYPE_MAP, zend_hash_num_elements(&map->keys));

  while (zend_hash_get_current_data(&map->keys, (void**) &current) == SUCCESS) {
    if (php_cassandra_collection_append(collection, *current, map->key_type)) {
      zend_hash_get_current_data(&map->values, (void**) &current);
      if (!php_cassandra_collection_append(collection, *current, map->value_type)) {
        result = 0;
        break;
      }
    } else {
      result = 0;
      break;
    }

    zend_hash_move_forward(&map->keys);
    zend_hash_move_forward(&map->values);
  }

  zend_hash_set_pointer(&map->keys, &keys_ptr);
  zend_hash_set_pointer(&map->values, &values_ptr);

  if (result)
    *collection_ptr = collection;
  else
    cass_collection_free(collection);

  return result;
}
