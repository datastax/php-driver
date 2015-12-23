#include "php_cassandra.h"
#include "future.h"

int
php_cassandra_future_wait_timed(CassFuture *future, zval *timeout TSRMLS_DC)
{
  cass_duration_t timeout_us;

  if (cass_future_ready(future)) return SUCCESS;

  if (timeout == NULL || Z_TYPE_P(timeout) == IS_NULL) {
    cass_future_wait(future);
  } else {
    if ((Z_TYPE_P(timeout) == IS_LONG && Z_LVAL_P(timeout) > 0)) {
      timeout_us = Z_LVAL_P(timeout) * 1000000;
    } else if ((Z_TYPE_P(timeout) == IS_DOUBLE && Z_DVAL_P(timeout) > 0)) {
      timeout_us = ceil(Z_DVAL_P(timeout) * 1000000);
    } else {
      INVALID_ARGUMENT_VALUE(timeout, "an positive number of seconds or null", FAILURE);
    }

    if (!cass_future_wait_timed(future, timeout_us)) {
      zend_throw_exception_ex(cassandra_timeout_exception_ce, 0 TSRMLS_CC,
                              "Future hasn't resolved within %f seconds", timeout_us / 1000000.0);
      return FAILURE;
    }
  }

  return SUCCESS;
}

int
php_cassandra_future_is_error(CassFuture *future TSRMLS_DC)
{
  int rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    const char *message;
    size_t      message_len;
    cass_future_error_message(future, &message, &message_len);
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
                            "%.*s", (int) message_len, message);
    return FAILURE;
  }
  return SUCCESS;
}
