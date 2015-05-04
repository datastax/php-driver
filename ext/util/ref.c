#include "php_cassandra.h"
#include "util/math.h"

cassandra_statement_ref*
php_cassandra_new_ref(CassStatement* statement)
{
  cassandra_statement_ref* ref =
    (cassandra_statement_ref*) emalloc(sizeof(cassandra_statement_ref));

  ref->statement = statement;
  ref->count     = 1;

  return ref;
}

cassandra_statement_ref*
php_cassandra_add_ref(cassandra_statement_ref* ref)
{
  ref->count++;
  return ref;
}

void
php_cassandra_del_ref(cassandra_statement_ref** ref_ptr)
{
  cassandra_statement_ref* ref = *ref_ptr;
  ref->count--;

  if (ref->count <= 0) {
    cass_statement_free(ref->statement);
    efree(ref);
    ref_ptr = NULL;
  }
}
