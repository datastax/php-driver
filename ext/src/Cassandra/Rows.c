#include "php_cassandra.h"
#include "util/future.h"
#include "util/ref.h"
#include "util/result.h"

zend_class_entry *cassandra_rows_ce = NULL;

static void
php_cassandra_rows_clear(cassandra_rows* self)
{
  if (self->result) {
    cass_result_free(self->result);
    self->result  = NULL;
  }

  if (self->statement) {
    php_cassandra_del_ref(&self->statement);
  }

  if (self->result) {
    cass_result_free(self->result);
    self->result = NULL;
  }

  if (self->session) {
    zval_ptr_dtor(&self->session);
    self->session = NULL;
  }
}

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
  cassandra_rows* self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(self->rows)));
}

PHP_METHOD(Rows, rewind)
{
  cassandra_rows* self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  zend_hash_internal_pointer_reset(Z_ARRVAL_P(self->rows));
}

PHP_METHOD(Rows, current)
{
  zval **entry;
  cassandra_rows* self = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_get_current_data(Z_ARRVAL_P(self->rows), (void **) &entry) == SUCCESS)
    RETURN_ZVAL(*entry, 1, 0);
}

PHP_METHOD(Rows, key)
{
  ulong key;
  char* string;
  cassandra_rows* self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (zend_hash_get_current_key(Z_ARRVAL_P(self->rows), &string, &key, 0) == HASH_KEY_IS_LONG)
    RETURN_LONG(key);
}

PHP_METHOD(Rows, next)
{
  cassandra_rows* self = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  zend_hash_move_forward(Z_ARRVAL_P(self->rows));
}

PHP_METHOD(Rows, valid)
{
  cassandra_rows* self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_BOOL(zend_hash_has_more_elements(Z_ARRVAL_P(self->rows)) == SUCCESS);
}

PHP_METHOD(Rows, offsetExists)
{
  zval* offset;
  cassandra_rows* self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  if (Z_TYPE_P(offset) != IS_LONG || Z_LVAL_P(offset) < 0) {
    INVALID_ARGUMENT(offset, "a positive integer");
  }

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_BOOL(zend_hash_index_exists(Z_ARRVAL_P(self->rows), (ulong) Z_LVAL_P(offset)));
}

PHP_METHOD(Rows, offsetGet)
{
  zval*  offset;
  zval** value;
  cassandra_rows* self = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE)
    return;

  if (Z_TYPE_P(offset) != IS_LONG || Z_LVAL_P(offset) < 0) {
    INVALID_ARGUMENT(offset, "a positive integer");
  }

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

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
  cassandra_rows* self = NULL;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->result           == NULL &&
      self->next_page        == NULL &&
      self->future_next_page == NULL) {
    RETURN_TRUE;
  }

  RETURN_FALSE;
}

PHP_METHOD(Rows, nextPage)
{
  zval* timeout = NULL;
  cassandra_session* session = NULL;
  CassFuture* future = NULL;
  const CassResult* result = NULL;
  cassandra_rows* rows = NULL;
  cassandra_future_rows* future_rows = NULL;

  cassandra_rows* self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->next_page) {
    RETURN_ZVAL(self->next_page, 1, 0);
  }

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  if (self->future_next_page) {
    if (!instanceof_function(Z_OBJCE_P(self->future_next_page),
                             cassandra_future_rows_ce TSRMLS_CC)) {
      zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC,
                              "Unexpected future instance.");
      return;
    }

    future_rows = (cassandra_future_rows*) zend_object_store_get_object(self->future_next_page TSRMLS_CC);

    if (php_cassandra_future_wait_timed(future_rows->future, timeout TSRMLS_CC) == FAILURE) {
      return;
    }

    if (php_cassandra_future_is_error(future_rows->future TSRMLS_CC) == FAILURE) {
      return;
    }

    result = cass_future_get_result(future_rows->future);
  } else {
    if (self->result == NULL) {
      return;
    }

    ASSERT_SUCCESS(cass_statement_set_paging_state((CassStatement*) self->statement->data, self->result));

    session = (cassandra_session*) zend_object_store_get_object(self->session TSRMLS_CC);
    future = cass_session_execute(session->session, (CassStatement*) self->statement->data);

    if (php_cassandra_future_wait_timed(future, timeout TSRMLS_CC) == FAILURE) {
      return;
    }

    if (php_cassandra_future_is_error(future TSRMLS_CC) == FAILURE) {
      return;
    }

    result = cass_future_get_result(future);
    cass_future_free(future);
  }

  if (!result) {
    zend_throw_exception_ex(cassandra_runtime_exception_ce, 0 TSRMLS_CC,
                            "Future doesn't contain a result.");
    return;
  }

  MAKE_STD_ZVAL(self->next_page);
  object_init_ex(self->next_page, cassandra_rows_ce);
  rows = (cassandra_rows*) zend_object_store_get_object(self->next_page TSRMLS_CC);

  if (php_cassandra_get_result(result, &rows->rows TSRMLS_CC) == FAILURE) {
    cass_result_free(result);
    zval_dtor(self->next_page);
    self->next_page = NULL;
    return;
  }

  if (self->future_next_page) {
    zval_ptr_dtor(&self->future_next_page);
    self->future_next_page = NULL;
  }

  if (cass_result_has_more_pages(result)) {
    Z_ADDREF_P(self->session);
    rows->statement = php_cassandra_add_ref(self->statement);
    rows->session   = self->session;
    rows->result    = result;
  } else {
    cass_result_free(result);
  }

  php_cassandra_rows_clear(self);
  RETURN_ZVAL(self->next_page, 1, 0);
}

PHP_METHOD(Rows, nextPageAsync)
{
  cassandra_rows* self = NULL;
  cassandra_session* session = NULL;
  CassFuture* future = NULL;
  cassandra_future_rows* future_rows = NULL;
  cassandra_future_value* future_value;

  if (zend_parse_parameters_none() == FAILURE)
    return;

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (self->future_next_page) {
    RETURN_ZVAL(self->future_next_page, 1, 0);
  }

  if (self->next_page) {
    Z_ADDREF_P(self->next_page);
    object_init_ex(self->future_next_page, cassandra_future_value_ce);
    future_value = (cassandra_future_value*) zend_object_store_get_object(self->future_next_page TSRMLS_CC);
    future_value->value = self->next_page;
    RETURN_ZVAL(self->future_next_page, 1, 0);
  }

  if (self->result == NULL) {
    object_init_ex(return_value, cassandra_future_value_ce);
    return;
  }

  ASSERT_SUCCESS(cass_statement_set_paging_state((CassStatement*) self->statement->data, self->result));

  session = (cassandra_session*) zend_object_store_get_object(self->session TSRMLS_CC);
  future = cass_session_execute(session->session, (CassStatement*) self->statement->data);

  object_init_ex(self->future_next_page, cassandra_future_rows_ce);
  future_rows = (cassandra_future_rows*) zend_object_store_get_object(self->future_next_page TSRMLS_CC);

  Z_ADDREF_P(self->session);
  future_rows->session   = self->session;
  future_rows->statement = php_cassandra_add_ref(self->statement);
  future_rows->future    = future;

  php_cassandra_rows_clear(self);
  RETURN_ZVAL(self->future_next_page, 1, 0);
}

PHP_METHOD(Rows, first)
{
  HashPointer ptr;
  zval **entry;
  cassandra_rows* self = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_rows*) zend_object_store_get_object(getThis() TSRMLS_CC);

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
  HashTable* props = zend_std_get_properties(object TSRMLS_CC);

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
  php_cassandra_rows_clear(self);

  if (self->rows) {
    zval_ptr_dtor(&self->rows);
    self->rows = NULL;
  }

  if (self->next_page) {
    zval_ptr_dtor(&self->next_page);
    self->next_page = NULL;
  }

  if (self->future_next_page) {
    zval_ptr_dtor(&self->future_next_page);
    self->future_next_page = NULL;
  }

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

  self->statement = NULL;
  self->result    = NULL;
  self->session   = NULL;
  self->rows      = NULL;
  self->next_page = NULL;

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
