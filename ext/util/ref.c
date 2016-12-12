/**
 * Copyright 2015-2016 DataStax, Inc.
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

cassandra_ref*
php_cassandra_new_peref(void *data, cassandra_free_function destructor, int persistent)
{
  cassandra_ref *ref = (cassandra_ref*) pemalloc(sizeof(cassandra_ref), persistent);

  ref->data     = data;
  ref->destruct = destructor;
  ref->count    = 1;

  return ref;
}

void
php_cassandra_del_peref(cassandra_ref **ref_ptr, int persistent)
{
  cassandra_ref *ref = *ref_ptr;
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
