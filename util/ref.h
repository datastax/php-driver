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

#ifndef PHP_DRIVER_REF_H
#define PHP_DRIVER_REF_H

php_driver_ref *php_driver_new_peref(void *data, php_driver_free_function destructor, int persistent);
void php_driver_del_peref(php_driver_ref **ref_ptr, int persistent);

#define php_driver_new_ref(data, destructor) php_driver_new_peref(data, destructor, 0)
#define php_driver_del_ref(ref) php_driver_del_peref(ref, 0)
#define php_driver_add_ref(ref) (++ref->count, ref)

#endif /* PHP_DRIVER_REF_H */
