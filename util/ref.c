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

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/math.h"

php_driver_ref*
php_driver_new_peref(void *data, php_driver_free_function destructor, int persistent)
{
  php_driver_ref *ref = (php_driver_ref*) pemalloc(sizeof(php_driver_ref), persistent);

  ref->data     = data;
  ref->destruct = destructor;
  ref->count    = 1;

  return ref;
}

void
php_driver_del_peref(php_driver_ref **ref_ptr, int persistent)
{
  php_driver_ref *ref = *ref_ptr;
  if (ref) {
    ref->count--;

    if (ref->count <= 0) {
      ref->destruct(ref->data);
      ref->data = NULL;
      pefree(ref, persistent);
      *ref_ptr = NULL;
    }
  }
}
