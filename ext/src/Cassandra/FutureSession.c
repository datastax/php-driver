#include "php_cassandra.h"

zend_class_entry *cassandra_future_session_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(FutureSession, get)
{
  zval* timeout = NULL;
  cass_duration_t timeout_us;

  cassandra_future_session* future =
    (cassandra_future_session*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (future->exception_message) {
    zend_throw_exception_ex(exception_class(future->exception_code),
      future->exception_code TSRMLS_CC, future->exception_message);
    return;
  }

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  if (!cass_future_ready(future->future)) {
    if (timeout == NULL || Z_TYPE_P(timeout) == IS_NULL) {
      cass_future_wait(future->future);
    } else {
      if ((Z_TYPE_P(timeout) == IS_LONG && Z_LVAL_P(timeout) > 0)) {
        timeout_us = Z_LVAL_P(timeout) * 1000000;
      } else if ((Z_TYPE_P(timeout) == IS_DOUBLE && Z_DVAL_P(timeout) > 0)) {
        timeout_us = ceil(Z_DVAL_P(timeout) * 1000000);
      } else {
        INVALID_ARGUMENT(timeout, "an positive number of seconds or null");
      }

      if (!cass_future_wait_timed(future->future, timeout_us)) {
        if (Z_TYPE_P(timeout) == IS_LONG) {
          zend_throw_exception_ex(cassandra_timeout_exception_ce, 0 TSRMLS_CC,
            "Unable to resolve future within %d seconds", Z_LVAL_P(timeout));
        } else {
          zend_throw_exception_ex(cassandra_timeout_exception_ce, 0 TSRMLS_CC,
            "Unable to resolve future within %f seconds", Z_DVAL_P(timeout));
        }
        return;
      }
    }
  }

  CassError rc = cass_future_error_code(future->future);

  if (rc != CASS_OK) {
    CassString message = cass_future_error_message(future->future);

    if (future->persist) {
      future->exception_message = estrndup(message.data, message.length);
      future->exception_code    = rc;

      if (zend_hash_del(&EG(persistent_list), future->hash_key, future->hash_key_len + 1) == SUCCESS) {
        future->session = NULL;
        future->future  = NULL;
      }

      zend_throw_exception_ex(exception_class(future->exception_code),
        future->exception_code TSRMLS_CC, future->exception_message);
      return;
    }

    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
      "%.*s", (int) message.length, message.data);
    return;
  }

  object_init_ex(return_value, cassandra_default_session_ce);
  cassandra_session* session =
    (cassandra_session*) zend_object_store_get_object(return_value TSRMLS_CC);
  session->session = future->session;
  session->persist = future->persist;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_future_session_methods[] = {
  PHP_ME(FutureSession, get, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_future_session_handlers;

static HashTable*
php_cassandra_future_session_properties(zval *object TSRMLS_DC)
{
  cassandra_future_session* future = (cassandra_future_session*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*                props  = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_future_session_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_future_session_free(void *object TSRMLS_DC)
{
  cassandra_future_session* future = (cassandra_future_session*) object;

  zend_object_std_dtor(&future->zval TSRMLS_CC);

  if (future->persist) {
    efree(future->hash_key);
  } else {
    cass_future_free(future->future);
    cass_session_free(future->session);
  }

  if (future->exception_message)
    efree(future->exception_message);

  efree(future);
}

static zend_object_value
php_cassandra_future_session_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_future_session *future;

  future = (cassandra_future_session*) ecalloc(1, sizeof(cassandra_future_session));
  future->session           = NULL;
  future->future            = NULL;
  future->exception_message = NULL;
  future->hash_key          = NULL;
  future->persist           = 0;

  zend_object_std_init(&future->zval, class_type TSRMLS_CC);
  object_properties_init(&future->zval, class_type);

  retval.handle   = zend_objects_store_put(future,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_future_session_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_future_session_handlers;

  return retval;
}

void cassandra_define_FutureSession(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\FutureSession", cassandra_future_session_methods);
  cassandra_future_session_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_future_session_ce TSRMLS_CC, 1, cassandra_future_ce);
  cassandra_future_session_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_future_session_ce->create_object = php_cassandra_future_session_new;

  memcpy(&cassandra_future_session_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_future_session_handlers.get_properties  = php_cassandra_future_session_properties;
  cassandra_future_session_handlers.compare_objects = php_cassandra_future_session_compare;
}
