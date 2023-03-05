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

#include <php_driver.h>
#include <php_driver_types.h>

#include <Zend/zend_smart_str.h>
#include <cassandra.h>

#define PHP_DRIVER_SCALAR_TYPES_MAP(XX)    \
  XX(ascii, CASS_VALUE_TYPE_ASCII)         \
  XX(bigint, CASS_VALUE_TYPE_BIGINT)       \
  XX(smallint, CASS_VALUE_TYPE_SMALL_INT)  \
  XX(tinyint, CASS_VALUE_TYPE_TINY_INT)    \
  XX(blob, CASS_VALUE_TYPE_BLOB)           \
  XX(boolean, CASS_VALUE_TYPE_BOOLEAN)     \
  XX(counter, CASS_VALUE_TYPE_COUNTER)     \
  XX(decimal, CASS_VALUE_TYPE_DECIMAL)     \
  XX(double, CASS_VALUE_TYPE_DOUBLE)       \
  XX(duration, CASS_VALUE_TYPE_DURATION)   \
  XX(float, CASS_VALUE_TYPE_FLOAT)         \
  XX(int, CASS_VALUE_TYPE_INT)             \
  XX(text, CASS_VALUE_TYPE_TEXT)           \
  XX(timestamp, CASS_VALUE_TYPE_TIMESTAMP) \
  XX(date, CASS_VALUE_TYPE_DATE)           \
  XX(time, CASS_VALUE_TYPE_TIME)           \
  XX(uuid, CASS_VALUE_TYPE_UUID)           \
  XX(varchar, CASS_VALUE_TYPE_VARCHAR)     \
  XX(varint, CASS_VALUE_TYPE_VARINT)       \
  XX(timeuuid, CASS_VALUE_TYPE_TIMEUUID)   \
  XX(inet, CASS_VALUE_TYPE_INET)

php5to7_zval php_driver_type_from_data_type(const CassDataType* data_type);

int php_driver_type_validate(zval* object, const char* object_name);
int php_driver_type_compare(php_driver_type* type1, php_driver_type* type2);
void php_driver_type_string(php_driver_type* type, smart_str* smart);

php5to7_zval php_driver_type_scalar(CassValueType type);
const char* php_driver_scalar_type_name(CassValueType type);

php5to7_zval php_driver_type_set(zval* value_type);
php5to7_zval php_driver_type_set_from_value_type(CassValueType type);

php5to7_zval php_driver_type_collection(zval* value_type);
php5to7_zval php_driver_type_collection_from_value_type(CassValueType type);

php5to7_zval php_driver_type_map(zval* key_type,
                                 zval* value_type);
php5to7_zval php_driver_type_map_from_value_types(CassValueType key_type,
                                                  CassValueType value_type);

php5to7_zval php_driver_type_tuple();

php5to7_zval php_driver_type_user_type();

php5to7_zval php_driver_type_custom(const char* name,
                                    size_t name_length);

int php_driver_parse_column_type(const char* validator,
                                 size_t validator_len,
                                 int* reversed_out,
                                 int* frozen_out,
                                 php5to7_zval* type_out);

void php_driver_scalar_init(INTERNAL_FUNCTION_PARAMETERS);
