#include <php.h>
// #include <php_spl.h>
// #include <spl_iterators.h>
#include <zend_exceptions.h>
#include "../php_cassandra.h"
#include "util/math.h"
#include "decimal.h"

extern zend_class_entry* spl_ce_Countable;
extern zend_class_entry* zend_ce_iterator;
extern zend_class_entry* cassandra_ce_RuntimeException;
extern zend_class_entry* cassandra_ce_InvalidArgumentException;

zend_class_entry *cassandra_ce_Set = NULL;

static int
php_cassandra_set_add(cassandra_set* set, zval* object)
{
  char* key;
  int   key_len;
  ulong hash;

  if (!php_cassandra_hash_object(object, set->type, &key, &key_len))
    return 0;

  hash = zend_get_hash_value(key, key_len);

  do {
    // element is in the set
    if (zend_hash_quick_exists(&set->values, key, key_len, hash))
      break;

    if (zend_hash_quick_add(&set->values, key, key_len, hash, (void*) &object, sizeof(zval*), NULL) == FAILURE)
      break;

    Z_ADDREF_P(object);

    efree(key);
    return 1;
  } while (0);

  efree(key);
  return 0;
}

static int
php_cassandra_set_del(cassandra_set* set, zval* object)
{
  char* key;
  int   key_len;

  if (!php_cassandra_hash_object(object, set->type, &key, &key_len))
    return 0;

  if (zend_hash_del(&set->values, key, key_len) == FAILURE) {
    efree(key);
    return 0;
  }

  efree(key);
  return 1;
}

/* {{{ Cassandra\Set::__construct(string) */
PHP_METHOD(CassandraSet, __construct)
{
  char *type;
  int type_len;
  cassandra_set* set;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &type, &type_len) == FAILURE) {
    return;
  }

  set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (strncmp("ascii", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_ASCII;
  } else if (strncmp("bigint", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_BIGINT;
  } else if (strncmp("blob", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_BLOB;
  } else if (strncmp("boolean", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_BOOLEAN;
  } else if (strncmp("counter", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_COUNTER;
  } else if (strncmp("decimal", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_DECIMAL;
  } else if (strncmp("double", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_DOUBLE;
  } else if (strncmp("float", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_FLOAT;
  } else if (strncmp("int", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_INT;
  } else if (strncmp("text", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_TEXT;
  } else if (strncmp("timestamp", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_TIMESTAMP;
  } else if (strncmp("uuid", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_UUID;
  } else if (strncmp("varchar", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_VARCHAR;
  } else if (strncmp("varint", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_VARINT;
  } else if (strncmp("timeuuid", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_TIMEUUID;
  } else if (strncmp("inet", type, type_len) == 0) {
    set->type = CASS_VALUE_TYPE_INET;
  } else {
    zend_throw_exception_ex(cassandra_ce_InvalidArgumentException, 0 TSRMLS_CC,
      "Unsupported type '%s'", type);
    return;
  }
}
/* }}} */

/* {{{ Cassandra\Set::__toString() */
PHP_METHOD(CassandraSet, __toString)
{
}
/* }}} */

/* {{{ Cassandra\Set::type() */
PHP_METHOD(CassandraSet, type)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  switch (set->type) {
  case CASS_VALUE_TYPE_TEXT:
    RETURN_STRING("text", 1);
  case CASS_VALUE_TYPE_ASCII:
    RETURN_STRING("ascii", 1);
  case CASS_VALUE_TYPE_VARCHAR:
    RETURN_STRING("varchar", 1);
  case CASS_VALUE_TYPE_BIGINT:
    RETURN_STRING("bigint", 1);
  case CASS_VALUE_TYPE_BLOB:
    RETURN_STRING("blob", 1);
  case CASS_VALUE_TYPE_BOOLEAN:
    RETURN_STRING("boolean", 1);
  case CASS_VALUE_TYPE_COUNTER:
    RETURN_STRING("counter", 1);
  case CASS_VALUE_TYPE_DECIMAL:
    RETURN_STRING("decimal", 1);
  case CASS_VALUE_TYPE_DOUBLE:
    RETURN_STRING("double", 1);
  case CASS_VALUE_TYPE_FLOAT:
    RETURN_STRING("float", 1);
  case CASS_VALUE_TYPE_INT:
    RETURN_STRING("int", 1);
  case CASS_VALUE_TYPE_TIMESTAMP:
    RETURN_STRING("timestamp", 1);
  case CASS_VALUE_TYPE_UUID:
    RETURN_STRING("uuid", 1);
  case CASS_VALUE_TYPE_VARINT:
    RETURN_STRING("varint", 1);
  case CASS_VALUE_TYPE_TIMEUUID:
    RETURN_STRING("timeuuid", 1);
  case CASS_VALUE_TYPE_INET:
    RETURN_STRING("inet", 1);
  }
}
/* }}} */

/* {{{ Cassandra\Set::toArray() */
PHP_METHOD(CassandraSet, toArray)
{
  HashPointer ptr;
  zval** current;
  array_init(return_value);
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  zend_hash_get_pointer(&set->values, &ptr);
  zend_hash_internal_pointer_reset(&set->values);

  while (zend_hash_get_current_data(&set->values, (void**) &current) == SUCCESS) {
    if (add_next_index_zval(return_value, *current) == SUCCESS)
      Z_ADDREF_PP(current);
    else
      break;

    zend_hash_move_forward(&set->values);
  }

  zend_hash_set_pointer(&set->values, &ptr);
}
/* }}} */

/* {{{ Cassandra\Set::add(value) */
PHP_METHOD(CassandraSet, add)
{
  zval* object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_set_add(set, object))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::remove(value) */
PHP_METHOD(CassandraSet, remove)
{
  zval* object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_set_del(set, object))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Set::count() */
PHP_METHOD(CassandraSet, count)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(zend_hash_num_elements(&set->values));
}
/* }}} */

/* {{{ Cassandra\Set::current() */
PHP_METHOD(CassandraSet, current)
{
  zval** current;
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_get_current_data(&set->values, (void**) &current) == SUCCESS)
    RETURN_ZVAL(*current, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Set::key() */
PHP_METHOD(CassandraSet, key)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(set->pos);
}
/* }}} */

/* {{{ Cassandra\Set::next() */
PHP_METHOD(CassandraSet, next)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  if (zend_hash_move_forward(&set->values) == SUCCESS)
    set->pos++;
}
/* }}} */

/* {{{ Cassandra\Set::valid() */
PHP_METHOD(CassandraSet, valid)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_hash_has_more_elements(&set->values) == SUCCESS);
}
/* }}} */

/* {{{ Cassandra\Set::rewind() */
PHP_METHOD(CassandraSet, rewind)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  zend_hash_internal_pointer_reset(&set->values);
  set->pos = 0;
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_one, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry CassandraSet_methods[] = {
  PHP_ME(CassandraSet, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, toArray, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, add, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, remove, arginfo_one, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(CassandraSet, count, arginfo_none, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(CassandraSet, current, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, key, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, next, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, valid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(CassandraSet, rewind, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static void
php_cassandra_set_free(void *object TSRMLS_DC)
{
  cassandra_set* set = (cassandra_set*) object;

  zend_hash_destroy(&set->values);
  zend_object_std_dtor(&set->zval TSRMLS_CC);

  efree(set);
}

static zend_object_value
php_cassandra_set_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_set *set;

  set = (cassandra_set*) emalloc(sizeof(cassandra_set));
  memset(set, 0, sizeof(cassandra_set));

  zend_hash_init(&set->values, 0, NULL, ZVAL_PTR_DTOR, 0);
  zend_object_std_init(&set->zval, class_type TSRMLS_CC);
  object_properties_init(&set->zval, class_type TSRMLS_CC);

  retval.handle   = zend_objects_store_put(set,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_set_free, NULL TSRMLS_CC);
  retval.handlers = zend_get_std_object_handlers();

  return retval;
}

void cassandra_define_CassandraSet(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Set", CassandraSet_methods);
  cassandra_ce_Set = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_ce_Set->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_ce_Set->create_object = php_cassandra_set_new;
  zend_class_implements(cassandra_ce_Set TSRMLS_CC, 2, spl_ce_Countable, zend_ce_iterator);
}
