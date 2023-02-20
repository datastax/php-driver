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

#ifndef PHP_DRIVER_UTIL_COLLECTIONS_H
#define PHP_DRIVER_UTIL_COLLECTIONS_H

int php_driver_validate_object(zval* object, zval* ztype TSRMLS_DC);
int php_driver_value_type(char* type, CassValueType* value_type TSRMLS_DC);

int php_driver_collection_from_set(php_driver_set* set, CassCollection** collection_ptr TSRMLS_DC);
int php_driver_collection_from_collection(php_driver_collection* coll, CassCollection** collection_ptr TSRMLS_DC);
int php_driver_collection_from_map(php_driver_map* map, CassCollection** collection_ptr TSRMLS_DC);

int php_driver_tuple_from_tuple(php_driver_tuple *tuple, CassTuple **output TSRMLS_DC);

int php_driver_user_type_from_user_type_value(php_driver_user_type_value *user_type_value, CassUserType **output TSRMLS_DC);

#endif /* PHP_DRIVER_UTIL_COLLECTIONS_H */
