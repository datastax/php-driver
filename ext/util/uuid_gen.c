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
#include "php_driver_globals.h"
#include <stdlib.h>
#include "util/uuid_gen.h"

static CassUuidGen* get_uuid_gen(TSRMLS_D) {
  /* Create a new uuid generator if our PID has changed. This prevents the same
   * UUIDs from being generated in forked processes.
   */
  if (PHP_DRIVER_G(uuid_gen_pid) != getpid()) {
    if (PHP_DRIVER_G(uuid_gen)) {
      cass_uuid_gen_free(PHP_DRIVER_G(uuid_gen));
    }
    PHP_DRIVER_G(uuid_gen) = cass_uuid_gen_new();
    PHP_DRIVER_G(uuid_gen_pid) = getpid();
  }
  return PHP_DRIVER_G(uuid_gen);
}

void
php_driver_uuid_generate_random(CassUuid *out TSRMLS_DC)
{
  CassUuidGen* uuid_gen = get_uuid_gen(TSRMLS_C);
  if (!uuid_gen) return;
  cass_uuid_gen_random(uuid_gen, out);
}

void
php_driver_uuid_generate_time(CassUuid *out TSRMLS_DC)
{
  CassUuidGen* uuid_gen = get_uuid_gen(TSRMLS_C);
  if (!uuid_gen) return;
  cass_uuid_gen_time(uuid_gen, out);
}

void
php_driver_uuid_generate_from_time(long timestamp, CassUuid *out TSRMLS_DC)
{
  CassUuidGen* uuid_gen = get_uuid_gen(TSRMLS_C);
  if (!uuid_gen) return;
  cass_uuid_gen_from_time(uuid_gen, (cass_uint64_t) timestamp, out);
}
