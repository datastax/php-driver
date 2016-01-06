#include "php_cassandra.h"
#include "util/math.h"

cassandra_ref*
php_cassandra_new_ref(void *data, cassandra_free_function destructor)
{
  cassandra_ref *ref = (cassandra_ref*) emalloc(sizeof(cassandra_ref));

  ref->data     = data;
  ref->destruct = destructor;
  ref->count    = 1;

  return ref;
}

cassandra_ref*
php_cassandra_add_ref(cassandra_ref *ref)
{
  ref->count++;
  return ref;
}

void
php_cassandra_del_ref(cassandra_ref **ref_ptr)
{
  cassandra_ref *ref = *ref_ptr;
  if (ref) {
    ref->count--;

    if (ref->count <= 0) {
      ref->destruct(ref->data);
      ref->data = NULL;
      efree(ref);
      *ref_ptr = NULL;
    }
  }
}
