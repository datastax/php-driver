#include "php_cassandra.h"
#include "result.h"
#include "math.h"
#include "collections.h"
#include "types.h"
#include "src/Cassandra/Collection.h"
#include "src/Cassandra/Map.h"
#include "src/Cassandra/Set.h"

static int
#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
php_cassandra_value(const CassValue* value, const CassDataType* data_type, zval** out TSRMLS_DC)
#else
php_cassandra_value(const CassValue* value, CassValueType type, zval** out TSRMLS_DC)
#endif
{
  zval* return_value;
  const char* v_string;
  size_t v_string_len;
  const cass_byte_t* v_bytes;
  size_t v_bytes_len;
  const cass_byte_t* v_decimal;
  size_t v_decimal_len;
  cass_int32_t v_decimal_scale;
  cass_int32_t v_int_32;
  cass_bool_t v_boolean;
  cass_double_t v_double;
  cassandra_uuid* uuid;
  CassIterator* iterator;
  cassandra_bigint* bigint_number = NULL;
  cassandra_timestamp* timestamp = NULL;
  cassandra_blob* blob = NULL;
  cassandra_varint* varint_number = NULL;
  cassandra_inet* inet = NULL;
  cassandra_decimal* decimal_number = NULL;
  cassandra_float* float_number = NULL;
  cassandra_collection* collection = NULL;
  cassandra_map* map = NULL;
  cassandra_set* set = NULL;

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
  CassValueType type = cass_data_type_type(data_type);
  const CassDataType* primary_type;
  const CassDataType* secondary_type;
#else
  CassValueType primary_type;
  CassValueType secondary_type;
#endif

  MAKE_STD_ZVAL(return_value);
  RETVAL_NULL();

  if (cass_value_is_null(value)) {
    *out = return_value;
    return SUCCESS;
  }

  switch (type) {
  case CASS_VALUE_TYPE_ASCII:
  case CASS_VALUE_TYPE_TEXT:
  case CASS_VALUE_TYPE_VARCHAR:
    ASSERT_SUCCESS_BLOCK(cass_value_get_string(value, &v_string, &v_string_len),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    );
    RETVAL_STRINGL(v_string, v_string_len, 1);
    break;
  case CASS_VALUE_TYPE_INT:
    ASSERT_SUCCESS_BLOCK(cass_value_get_int32(value, &v_int_32),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    );
    RETVAL_LONG(v_int_32);
    break;
  case CASS_VALUE_TYPE_COUNTER:
  case CASS_VALUE_TYPE_BIGINT:
    object_init_ex(return_value, cassandra_bigint_ce);
    bigint_number = (cassandra_bigint*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_int64(value, &bigint_number->value),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    )
    break;
  case CASS_VALUE_TYPE_TIMESTAMP:
    object_init_ex(return_value, cassandra_timestamp_ce);
    timestamp = (cassandra_timestamp*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_int64(value, &timestamp->timestamp),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    )
    break;
  case CASS_VALUE_TYPE_BLOB:
    object_init_ex(return_value, cassandra_blob_ce);
    blob = (cassandra_blob*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_bytes(value, &v_bytes, &v_bytes_len),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    )
    blob->data = emalloc(v_bytes_len * sizeof(cass_byte_t));
    blob->size = v_bytes_len;
    memcpy(blob->data, v_bytes, v_bytes_len);
    break;
  case CASS_VALUE_TYPE_VARINT:
    object_init_ex(return_value, cassandra_varint_ce);
    varint_number = (cassandra_varint*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_bytes(value, &v_bytes, &v_bytes_len),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    );
    import_twos_complement((cass_byte_t*) v_bytes, v_bytes_len, &varint_number->value);
    break;
  case CASS_VALUE_TYPE_UUID:
    object_init_ex(return_value, cassandra_uuid_ce);
    uuid = (cassandra_uuid*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_uuid(value, &uuid->uuid),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    )
    break;
  case CASS_VALUE_TYPE_TIMEUUID:
    object_init_ex(return_value, cassandra_timeuuid_ce);
    uuid = (cassandra_uuid*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_uuid(value, &uuid->uuid),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    )
    break;
  case CASS_VALUE_TYPE_BOOLEAN:
    ASSERT_SUCCESS_BLOCK(cass_value_get_bool(value, &v_boolean),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    );
    if (v_boolean) {
      RETVAL_TRUE;
    } else {
      RETVAL_FALSE;
    }
    break;
  case CASS_VALUE_TYPE_INET:
    object_init_ex(return_value, cassandra_inet_ce);
    inet = (cassandra_inet*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_inet(value, &inet->inet),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    )
    break;
  case CASS_VALUE_TYPE_DECIMAL:
    object_init_ex(return_value, cassandra_decimal_ce);
    decimal_number = (cassandra_decimal*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_decimal(value, &v_decimal, &v_decimal_len, &v_decimal_scale),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    );
    import_twos_complement((cass_byte_t*) v_decimal, v_decimal_len, &decimal_number->value);
    decimal_number->scale = v_decimal_scale;
    break;
  case CASS_VALUE_TYPE_DOUBLE:
    ASSERT_SUCCESS_BLOCK(cass_value_get_double(value, &v_double),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    );
    RETVAL_DOUBLE(v_double);
    break;
  case CASS_VALUE_TYPE_FLOAT:
    object_init_ex(return_value, cassandra_float_ce);
    float_number = (cassandra_float*) zend_object_store_get_object(return_value TSRMLS_CC);
    ASSERT_SUCCESS_BLOCK(cass_value_get_float(value, &float_number->value),
      zval_ptr_dtor(&return_value);
      return FAILURE;
    )
    break;
  case CASS_VALUE_TYPE_LIST:
    object_init_ex(return_value, cassandra_collection_ce);
    collection = (cassandra_collection*) zend_object_store_get_object(return_value TSRMLS_CC);

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
    primary_type = cass_data_type_sub_data_type(data_type, 0);
    collection->type = php_cassandra_type_from_data_type(data_type TSRMLS_CC);
#else
    primary_type = cass_value_primary_sub_type(value);
    collection->type = php_cassandra_type_collection_from_value_type(primary_type);
#endif

    iterator = cass_iterator_from_collection(value);

    while (cass_iterator_next(iterator)) {
      zval *v;

      if (php_cassandra_value(cass_iterator_get_value(iterator), primary_type, &v TSRMLS_CC) == FAILURE) {
        cass_iterator_free(iterator);
        zval_ptr_dtor(&return_value);
        return FAILURE;
      }

      php_cassandra_collection_add(collection, v TSRMLS_CC);
      zval_ptr_dtor(&v);
    }

    cass_iterator_free(iterator);
    break;
  case CASS_VALUE_TYPE_MAP:
    object_init_ex(return_value, cassandra_map_ce);
    map = (cassandra_map*) zend_object_store_get_object(return_value TSRMLS_CC);

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
    primary_type = cass_data_type_sub_data_type(data_type, 0);
    secondary_type = cass_data_type_sub_data_type(data_type, 1);
    map->type = php_cassandra_type_from_data_type(data_type TSRMLS_CC);
#else
    primary_type = cass_value_primary_sub_type(value);
    secondary_type = cass_value_secondary_sub_type(value);
    map->type = php_cassandra_type_map_from_value_types(primary_type, secondary_type);
#endif

    iterator = cass_iterator_from_map(value);

    while (cass_iterator_next(iterator)) {
      zval* k;
      zval* v;

      if (php_cassandra_value(cass_iterator_get_map_key(iterator), primary_type, &k TSRMLS_CC) == FAILURE ||
          php_cassandra_value(cass_iterator_get_map_value(iterator), secondary_type, &v TSRMLS_CC) == FAILURE) {
        cass_iterator_free(iterator);
        zval_ptr_dtor(&return_value);
        return FAILURE;
      }

      php_cassandra_map_set(map, k, v TSRMLS_CC);
      zval_ptr_dtor(&k);
      zval_ptr_dtor(&v);
    }

    cass_iterator_free(iterator);
    break;
  case CASS_VALUE_TYPE_SET:
    object_init_ex(return_value, cassandra_set_ce);
    set = (cassandra_set*) zend_object_store_get_object(return_value TSRMLS_CC);

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
    primary_type = cass_data_type_sub_data_type(data_type, 0);
    set->type = php_cassandra_type_from_data_type(data_type TSRMLS_CC);
#else
    primary_type = cass_value_primary_sub_type(value);
    secondary_type = cass_value_secondary_sub_type(value);
    set->type = php_cassandra_type_set_from_value_type(primary_type);
#endif

    iterator = cass_iterator_from_collection(value);

    while (cass_iterator_next(iterator)) {
      zval* v;

      if (php_cassandra_value(cass_iterator_get_value(iterator), primary_type, &v TSRMLS_CC) == FAILURE) {
        cass_iterator_free(iterator);
        zval_ptr_dtor(&return_value);
        return FAILURE;
      }

      php_cassandra_set_add(set, v TSRMLS_CC);
      zval_ptr_dtor(&v);
    }

    cass_iterator_free(iterator);
    break;
  default:
    RETVAL_NULL();
  }

  *out = return_value;
  return SUCCESS;
}

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 2, 0)
int
php_cassandra_get_keyspace_field(const CassKeyspaceMeta* metadata, const char* field_name, zval** out TSRMLS_DC)
{
  const CassValue* value;

  value = cass_keyspace_meta_field_by_name(metadata, field_name);

  if (value == NULL) {
    MAKE_STD_ZVAL(*out);
    ZVAL_NULL(*out);
    return SUCCESS;
  }

  return php_cassandra_value(value, cass_value_data_type(value), out TSRMLS_CC);
}

int
php_cassandra_get_table_field(const CassTableMeta* metadata, const char* field_name, zval** out TSRMLS_DC)
{
  const CassValue* value;

  value = cass_table_meta_field_by_name(metadata, field_name);

  if (value == NULL) {
    MAKE_STD_ZVAL(*out);
    ZVAL_NULL(*out);
    return SUCCESS;
  }

  return php_cassandra_value(value, cass_value_data_type(value), out TSRMLS_CC);
}

int
php_cassandra_get_column_field(const CassColumnMeta* metadata, const char* field_name, zval** out TSRMLS_DC)
{
  const CassValue* value;

  value = cass_column_meta_field_by_name(metadata, field_name);

  if (value == NULL) {
    MAKE_STD_ZVAL(*out);
    ZVAL_NULL(*out);
    return SUCCESS;
  }

  return php_cassandra_value(value, cass_value_data_type(value), out TSRMLS_CC);
}
#else
int
php_cassandra_get_schema_field(const CassSchemaMeta* metadata, const char* field_name, zval** out TSRMLS_DC)
{
  const CassSchemaMetaField* field;
  const CassValue* value;

  field = cass_schema_meta_get_field(metadata, field_name);

  if (field == NULL) {
    MAKE_STD_ZVAL(*out);
    ZVAL_NULL(*out);
    return SUCCESS;
  }

  value = cass_schema_meta_field_value(field);

  if (value == NULL) {
    MAKE_STD_ZVAL(*out);
    ZVAL_NULL(*out);
    return SUCCESS;
  }

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
  return php_cassandra_value(value, cass_value_data_type(value), out TSRMLS_CC);
#else
  return php_cassandra_value(value, cass_value_type(value), out TSRMLS_CC);
#endif
}
#endif

int
php_cassandra_get_result(const CassResult* result, zval** out TSRMLS_DC)
{
  zval*            rows;
  zval*            row;
  const CassRow*   cass_row;
  const char*      column_name;
  size_t           column_name_len;
#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
  const CassDataType* column_type;
#else
  CassValueType    column_type;
#endif
  const CassValue* column_value;
  CassIterator*    iterator = NULL;
  size_t           columns = -1;
  char**           column_names;
  unsigned         i;

  MAKE_STD_ZVAL(rows);
  array_init(rows);

  iterator = cass_iterator_from_result(result);
  columns  = cass_result_column_count(result);

  column_names = (char**) ecalloc(columns, sizeof(char*));

  while (cass_iterator_next(iterator)) {
    MAKE_STD_ZVAL(row);
    array_init(row);
    cass_row = cass_iterator_get_row(iterator);

    for (i = 0; i < columns; i++) {
      zval* php_value;

      if (column_names[i] == NULL) {
        cass_result_column_name(result, i, &column_name, &column_name_len);
        column_names[i] = estrndup(column_name, column_name_len);
      }

#if CURRENT_CPP_DRIVER_VERSION >= CPP_DRIVER_VERSION(2, 1, 0)
      column_type  = cass_result_column_data_type(result, i);
#else
      column_type  = cass_result_column_type(result, i);
#endif

      column_value = cass_row_get_column(cass_row, i);

      if (php_cassandra_value(column_value, column_type, &php_value TSRMLS_CC) == FAILURE) {
        zval_ptr_dtor(&row);
        zval_ptr_dtor(&rows);

        for (i = 0; i < columns; i++) {
          if (column_names[i]) {
            efree(column_names[i]);
          }
        }

        efree(column_names);
        cass_iterator_free(iterator);

        return FAILURE;
      }

      add_assoc_zval_ex(row, column_names[i], strlen(column_names[i]) + 1, php_value);
    }

    add_next_index_zval(rows, row);
  }

  for (i = 0; i < columns; i++) {
    if (column_names[i] != NULL) {
      efree(column_names[i]);
    }
  }

  efree(column_names);
  cass_iterator_free(iterator);

  *out = rows;

  return SUCCESS;
}
