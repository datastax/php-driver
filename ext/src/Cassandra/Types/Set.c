#include "php_cassandra.h"
#include "util/collections.h"
#include "Set.h"

zend_class_entry *cassandra_set_ce = NULL;

int
php_cassandra_set_add(cassandra_set* set, zval* object TSRMLS_DC)
{
  char* key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(object, set->type, &key, &key_len TSRMLS_CC))
    return 0;

  if (zend_hash_add(&set->values, key, key_len, (void*) &object, sizeof(zval*), NULL) == SUCCESS) {
    Z_ADDREF_P(object);
    result = 1;
  }

  efree(key);
  return result;
}

static int
php_cassandra_set_del(cassandra_set* set, zval* object TSRMLS_DC)
{
  char* key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(object, set->type, &key, &key_len TSRMLS_CC))
    return 0;

  if (zend_hash_del(&set->values, key, key_len) == SUCCESS)
    result = 1;

  efree(key);
  return result;
}

static int
php_cassandra_set_has(cassandra_set* set, zval* object TSRMLS_DC)
{
  char* key;
  int   key_len;
  int   result = 0;

  if (!php_cassandra_hash_object(object, set->type, &key, &key_len TSRMLS_CC))
    return 0;

  result = zend_hash_exists(&set->values, key, key_len);

  efree(key);
  return result;
}

static void
php_cassandra_set_populate(cassandra_set* set, zval* array)
{
  HashPointer ptr;
  zval** current;

  zend_hash_get_pointer(&set->values, &ptr);
  zend_hash_internal_pointer_reset(&set->values);

  while (zend_hash_get_current_data(&set->values, (void**) &current) == SUCCESS) {
    if (add_next_index_zval(array, *current) == SUCCESS)
      Z_ADDREF_PP(current);
    else
      break;

    zend_hash_move_forward(&set->values);
  }

  zend_hash_set_pointer(&set->values, &ptr);
}

/* {{{ Cassandra\Types\Set::__construct(string) */
PHP_METHOD(Set, __construct)
{
  char *type;
  int type_len;
  cassandra_set* set;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &type, &type_len) == FAILURE) {
    return;
  }

  set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  php_cassandra_value_type(type, &set->type TSRMLS_CC);
}
/* }}} */

/* {{{ Cassandra\Types\Set::type() */
PHP_METHOD(Set, type)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_STRING(php_cassandra_type_name(set->type), 1);
}
/* }}} */

/* {{{ Cassandra\Types\Set::values() */
PHP_METHOD(Set, values)
{
  array_init(return_value);
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  php_cassandra_set_populate(set, return_value);
}
/* }}} */

/* {{{ Cassandra\Types\Set::add(value) */
PHP_METHOD(Set, add)
{
  cassandra_set* set = NULL;

  zval* object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_set_add(set, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Types\Set::remove(value) */
PHP_METHOD(Set, remove)
{
  cassandra_set* set = NULL;

  zval* object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_set_del(set, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Types\Set::has(value) */
PHP_METHOD(Set, has)
{
  cassandra_set* set = NULL;

  zval* object;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE)
    return;

  set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_set_has(set, object TSRMLS_CC))
    RETURN_TRUE;

  RETURN_FALSE;
}
/* }}} */

/* {{{ Cassandra\Types\Set::count() */
PHP_METHOD(Set, count)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(zend_hash_num_elements(&set->values));
}
/* }}} */

/* {{{ Cassandra\Types\Set::current() */
PHP_METHOD(Set, current)
{
  zval** current;
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_get_current_data(&set->values, (void**) &current) == SUCCESS)
    RETURN_ZVAL(*current, 1, 0);
}
/* }}} */

/* {{{ Cassandra\Types\Set::key() */
PHP_METHOD(Set, key)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_LONG(set->pos);
}
/* }}} */

/* {{{ Cassandra\Types\Set::next() */
PHP_METHOD(Set, next)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  if (zend_hash_move_forward(&set->values) == SUCCESS)
    set->pos++;
}
/* }}} */

/* {{{ Cassandra\Types\Set::valid() */
PHP_METHOD(Set, valid)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  RETURN_BOOL(zend_hash_has_more_elements(&set->values) == SUCCESS);
}
/* }}} */

/* {{{ Cassandra\Types\Set::rewind() */
PHP_METHOD(Set, rewind)
{
  cassandra_set* set = (cassandra_set*) zend_object_store_get_object(getThis() TSRMLS_CC);
  zend_hash_internal_pointer_reset(&set->values);
  set->pos = 0;
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_one, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_set_methods[] = {
  PHP_ME(Set, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Set, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, values, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, add, arginfo_one, ZEND_ACC_PUBLIC)
  PHP_ME(Set, remove, arginfo_one, ZEND_ACC_PUBLIC)
  /* Countable */
  PHP_ME(Set, count, arginfo_none, ZEND_ACC_PUBLIC)
  /* Iterator */
  PHP_ME(Set, current, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, key, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, next, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, valid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Set, rewind, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_set_handlers;

static HashTable*
php_cassandra_set_properties(zval *object TSRMLS_DC)
{
  zval* values;

  cassandra_set* set   = (cassandra_set*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*     props = zend_std_get_properties(object TSRMLS_CC);

  MAKE_STD_ZVAL(values);
  array_init(values);

  php_cassandra_set_populate(set, values);

  zend_hash_update(props, "values", sizeof("values"), &values, sizeof(zval), NULL);

  return props;
}

int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC);

static int
php_cassandra_set_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  cassandra_set* set1 = NULL;
  cassandra_set* set2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  set1 = (cassandra_set*) zend_object_store_get_object(obj1 TSRMLS_CC);
  set2 = (cassandra_set*) zend_object_store_get_object(obj2 TSRMLS_CC);

  if (set1->type != set2->type)
    return 1;

  return zend_compare_symbol_tables_i(&set1->values, &set2->values TSRMLS_CC);
}

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
#if ZEND_MODULE_API_NO >= 20100525
  object_properties_init(&set->zval, class_type);
#else
  zend_hash_copy(set->zval.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void*) NULL, sizeof(zval*));
#endif

  retval.handle   = zend_objects_store_put(set,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_set_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_set_handlers;

  return retval;
}

void cassandra_define_Set(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Types\\Set", cassandra_set_methods);
  cassandra_set_ce = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&cassandra_set_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_set_handlers.get_properties = php_cassandra_set_properties;
  cassandra_set_handlers.compare_objects = php_cassandra_set_compare;
  cassandra_set_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;
  cassandra_set_ce->create_object = php_cassandra_set_new;
  zend_class_implements(cassandra_set_ce TSRMLS_CC, 2, spl_ce_Countable, zend_ce_iterator);
}
