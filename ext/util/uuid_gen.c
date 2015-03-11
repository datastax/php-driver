#include <php.h>
#include "../php_cassandra.h"
#include <stdlib.h>
#include "util/uuid_gen.h"

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

void
php_cassandra_uuid_generate_random(CassUuid* out)
{
  cass_uuid_gen_random(CASSANDRA_G(uuid_gen), out);
}

void
php_cassandra_uuid_generate_time(CassUuid* out)
{
  cass_uuid_gen_time(CASSANDRA_G(uuid_gen), out);
}

void
php_cassandra_uuid_generate_from_time(long timestamp, CassUuid* out)
{
  cass_uuid_gen_from_time(CASSANDRA_G(uuid_gen), (cass_uint64_t) timestamp, out);
}
