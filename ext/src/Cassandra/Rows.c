#include "php_cassandra.h"
#include "util/future.h"
#include "util/ref.h"

zend_class_entry *cassandra_rows_ce = NULL;
extern zend_class_entry *zend_ce_iterator;
extern zend_class_entry *zend_ce_arrayaccess;

PHP_METHOD(Rows, __construct)
{
  zend_throw_exception_ex(cassandra_logic_exception_ce, 0 TSRMLS_CC,
    "Instantiation of a Cassandra\\Rows objects directly is not supported, " \
    "call Cassandra\\Session::execute() or Cassandra\\FutureRows::get() instead."
  );
  return;
}

PHP_METHOD(Rows, count)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(self->rows)));
}

PHP_METHOD(Rows, rewind)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  zend_hash_internal_pointer_reset(Z_ARRVAL_P(self->rows));
}

PHP_METHOD(Rows, current)
{
  zval **entry;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_get_current_data(Z_ARRVAL_P(self->rows), (void **) &entry) == SUCCESS)
    RETURN_ZVAL(*entry, 1, 0);
}

PHP_METHOD(Rows, key)
{
  ulong key;
  char* string;
  int length;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_get_current_key(Z_ARRVAL_P(self->rows), &string, &key, 0) == HASH_KEY_IS_LONG)
    RETURN_LONG(key);
}

PHP_METHOD(Rows, next)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  zend_hash_move_forward(Z_ARRVAL_P(self->rows));
}

PHP_METHOD(Rows, valid)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_BOOL(zend_hash_has_more_elements(Z_ARRVAL_P(self->rows)) == SUCCESS);
}

PHP_METHOD(Rows, offsetExists)
{
  zval* offset;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  if (Z_TYPE_P(offset) != IS_LONG || Z_LVAL_P(offset) < 0) {
    INVALID_ARGUMENT(offset, "an positive integer");
  }

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_BOOL(zend_hash_index_exists(Z_ARRVAL_P(self->rows), (ulong) Z_LVAL_P(offset)));
}

PHP_METHOD(Rows, offsetGet)
{
  zval*  offset;
  zval** value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  if (Z_TYPE_P(offset) != IS_LONG || Z_LVAL_P(offset) < 0) {
    INVALID_ARGUMENT(offset, "an positive integer");
  }

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_index_find(Z_ARRVAL_P(self->rows), (ulong) Z_LVAL_P(offset), (void**) &value) == SUCCESS)
    RETURN_ZVAL(*value, 1, 0);
}

PHP_METHOD(Rows, offsetSet)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  zend_throw_exception_ex(cassandra_domain_exception_ce, 0 TSRMLS_CC,
    "Cannot overwrite a row at a given offset, rows are immutable."
  );
  return;
}

PHP_METHOD(Rows, offsetUnset)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  zend_throw_exception_ex(cassandra_domain_exception_ce, 0 TSRMLS_CC,
    "Cannot delete a row at a given offset, rows are immutable."
  );
  return;
}

PHP_METHOD(Rows, isLastPage)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (cass_result_has_more_pages(self->result))
    RETURN_TRUE;

  RETURN_FALSE;
}

PHP_METHOD(Rows, nextPage)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);
  if (!cass_result_has_more_pages(self->result)) {
    return;
  }

  // TODO: implement Rows::nextPage()
}

PHP_METHOD(Rows, nextPageAsync)
{
  if (zend_parse_parameters_none() == FAILURE)
    return;

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (!cass_result_has_more_pages(self->result)) {
    object_init_ex(return_value, cassandra_future_value_ce);
    return;
  }

  ASSERT_SUCCESS(cass_statement_set_paging_state(self->statement->statement, self->result));

  cassandra_session* session =
    (cassandra_session*) zend_object_store_get_object(self->session TSRMLS_CC);
  CassFuture* future = cass_session_execute(session->session, self->statement->statement);

  object_init_ex(return_value, cassandra_future_rows_ce);
  cassandra_future_rows* future_rows =
    (cassandra_future_rows*) zend_object_store_get_object(return_value TSRMLS_CC);

  Z_ADDREF_P(self->session);
  future_rows->session   = self->session;
  future_rows->statement = php_cassandra_add_ref(self->statement);
  future_rows->future    = future;
}

PHP_METHOD(Rows, first)
{
  HashPointer ptr;
  zval **entry;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  zend_hash_get_pointer(Z_ARRVAL_P(self->rows), &ptr);
  zend_hash_internal_pointer_reset(Z_ARRVAL_P(self->rows));

  if (zend_hash_get_current_data(Z_ARRVAL_P(self->rows), (void **) &entry) == SUCCESS)
    RETVAL_ZVAL(*entry, 1, 0);

  zend_hash_set_pointer(Z_ARRVAL_P(self->rows), &ptr);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_offset, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, offset)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_rows_methods[] = {
  PHP_ME(Rows, __construct,   arginfo_none,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Rows, count,         arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(Rows, rewind,        arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(Rows, current,       arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(Rows, key,           arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(Rows, next,          arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(Rows, valid,         arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(Rows, offsetExists,  arginfo_offset,  ZEND_ACC_PUBLIC)
  PHP_ME(Rows, offsetGet,     arginfo_offset,  ZEND_ACC_PUBLIC)
  PHP_ME(Rows, offsetSet,     arginfo_set,     ZEND_ACC_PUBLIC)
  PHP_ME(Rows, offsetUnset,   arginfo_offset,  ZEND_ACC_PUBLIC)
  PHP_ME(Rows, isLastPage,    arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(Rows, nextPage,      arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(Rows, nextPageAsync, arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_ME(Rows, first,         arginfo_none,    ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_rows_handlers;

static HashTable*
php_cassandra_rows_properties(zval *object TSRMLS_DC)
{
  cassandra_rows* self  = (cassandra_rows*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*      props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_rows_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_rows_free(void *object TSRMLS_DC)
{
  cassandra_rows* self = (cassandra_rows*) object;

  zend_object_std_dtor(&self->zval TSRMLS_CC);

  if (self->statement)
    php_cassandra_del_ref(&self->statement);

  if (self->result)
    cass_result_free(self->result);

  if (self->session)
    zval_ptr_dtor(&self->session);

  if (self->rows)
    zval_ptr_dtor(&self->rows);

  efree(self);
}

static zend_object_value
php_cassandra_rows_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_rows *self;

  self = (cassandra_rows*) ecalloc(1, sizeof(cassandra_rows));

  zend_object_std_init(&self->zval, class_type TSRMLS_CC);
  object_properties_init(&self->zval, class_type);

  ALLOC_ZVAL(self->rows);
  array_init(self->rows);

  self->statement = NULL;
  self->result    = NULL;
  self->session   = NULL;

  retval.handle   = zend_objects_store_put(self,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_rows_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_rows_handlers;

  return retval;
}

void cassandra_define_Rows(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Rows", cassandra_rows_methods);
  cassandra_rows_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_rows_ce TSRMLS_CC, 2, zend_ce_iterator, zend_ce_arrayaccess);
  cassandra_rows_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_rows_ce->create_object = php_cassandra_rows_new;

  memcpy(&cassandra_rows_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_rows_handlers.get_properties  = php_cassandra_rows_properties;
  cassandra_rows_handlers.compare_objects = php_cassandra_rows_compare;
}
