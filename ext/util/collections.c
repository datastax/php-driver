#include <php.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include <stdlib.h>
#include "util/collections.h"

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

extern zend_class_entry* cassandra_ce_InvalidArgumentException;
extern zend_class_entry* cassandra_ce_Bigint;
extern zend_class_entry* cassandra_ce_Blob;
extern zend_class_entry* cassandra_ce_Decimal;
extern zend_class_entry* cassandra_ce_Inet;
extern zend_class_entry* cassandra_ce_Timestamp;
extern zend_class_entry* cassandra_ce_Timeuuid;
extern zend_class_entry* cassandra_ce_Uuid;
extern zend_class_entry* cassandra_ce_Varint;

int
php_cassandra_validate_object(zval* object, CassValueType type)
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
  case CASS_VALUE_TYPE_FLOAT:
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
  }

  return 0;
}

int
php_cassandra_hash_object(zval* object, CassValueType type, char** key, int* len)
{
  zval* value;
  zval* scale;
  char* class_name = NULL;
  zend_uint class_name_len;

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
  case CASS_VALUE_TYPE_FLOAT:
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
  case CASS_VALUE_TYPE_COUNTER:
  case CASS_VALUE_TYPE_BIGINT:
    if (!INSTANCE_OF(cassandra_ce_Bigint)) {
      EXPECTING_VALUE("an instance of Cassandra\\Bigint");
    }

    value = zend_read_property(cassandra_ce_Bigint, object, "value", strlen("value"), 0 TSRMLS_CC);
    *len  = spprintf(key, 0, "C:BIGINT:%s", Z_STRVAL_P(value));
    return 1;
  case CASS_VALUE_TYPE_BLOB:
    if (!INSTANCE_OF(cassandra_ce_Blob)) {
      EXPECTING_VALUE("an instance of Cassandra\\Blob");
    }

    value = zend_read_property(cassandra_ce_Blob, object, "bytes", strlen("bytes"), 0 TSRMLS_CC);
    *len  = spprintf(key, 0, "C:BLOB:%s", Z_STRVAL_P(value));
    return 1;
  case CASS_VALUE_TYPE_DECIMAL:
    if (!INSTANCE_OF(cassandra_ce_Decimal)) {
      EXPECTING_VALUE("an instance of Cassandra\\Decimal");
    }

    value = zend_read_property(cassandra_ce_Decimal, object, "value", strlen("value"), 0 TSRMLS_CC);
    scale = zend_read_property(cassandra_ce_Decimal, object, "scale", strlen("scale"), 0 TSRMLS_CC);
    *len  = spprintf(key, 0, "C:DECIMAL:%s:%ld", Z_STRVAL_P(value), Z_LVAL_P(scale));
    return 1;
  case CASS_VALUE_TYPE_TIMESTAMP:
    if (!INSTANCE_OF(cassandra_ce_Timestamp)) {
      EXPECTING_VALUE("an instance of Cassandra\\Timestamp");
    }

    value = zend_read_property(cassandra_ce_Timestamp, object, "seconds", strlen("seconds"), 0 TSRMLS_CC);
    scale = zend_read_property(cassandra_ce_Timestamp, object, "microseconds", strlen("microseconds"), 0 TSRMLS_CC);
    *len  = spprintf(key, 0, "C:TIMESTAMP:%ld:%ld", Z_LVAL_P(value), Z_LVAL_P(scale));
    return 1;
  case CASS_VALUE_TYPE_UUID:
    if (!INSTANCE_OF(cassandra_ce_Uuid)) {
      EXPECTING_VALUE("an instance of Cassandra\\Uuid");
    }

    value = zend_read_property(cassandra_ce_Uuid, object, "uuid", strlen("uuid"), 0 TSRMLS_CC);
    *len  = spprintf(key, 0, "C:UUID:%s", Z_STRVAL_P(value));
    return 1;
  case CASS_VALUE_TYPE_VARINT:
    if (!INSTANCE_OF(cassandra_ce_Varint)) {
      EXPECTING_VALUE("an instance of Cassandra\\Varint");
    }

    value = zend_read_property(cassandra_ce_Varint, object, "value", strlen("value"), 0 TSRMLS_CC);
    *len  = spprintf(key, 0, "C:VARINT:%s", Z_STRVAL_P(value));
    return 1;
  case CASS_VALUE_TYPE_TIMEUUID:
    if (!INSTANCE_OF(cassandra_ce_Timeuuid)) {
      EXPECTING_VALUE("an instance of Cassandra\\Timeuuid");
    }

    value = zend_read_property(cassandra_ce_Timeuuid, object, "uuid", strlen("uuid"), 0 TSRMLS_CC);
    *len  = spprintf(key, 0, "C:TIMEUUID:%s", Z_STRVAL_P(value));
    return 1;
  case CASS_VALUE_TYPE_INET:
    if (!INSTANCE_OF(cassandra_ce_Inet)) {
      EXPECTING_VALUE("an instance of Cassandra\\Inet");
    }

    value = zend_read_property(cassandra_ce_Inet, object, "address", strlen("address"), 0 TSRMLS_CC);
    *len  = spprintf(key, 0, "C:INET:%s", Z_STRVAL_P(value));
    return 1;
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
