#include "php_cassandra.h"

#include <cassandra.h>
#include <php.h>
#include <Zend/zend_exceptions.h>

#include "future.h"

int
php_cassandra_future_wait_timed(CassFuture* future, zval* timeout TSRMLS_DC)
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
      if (Z_TYPE_P(timeout) == IS_LONG) {
        zend_throw_exception_ex(cassandra_timeout_exception_ce, 0 TSRMLS_CC,
                                "Unable to resolve future within %d seconds", Z_LVAL_P(timeout));
        return FAILURE;
      } else {
        zend_throw_exception_ex(cassandra_timeout_exception_ce, 0 TSRMLS_CC,
                                "Unable to resolve future within %f seconds", Z_DVAL_P(timeout));
      }
      return FAILURE;
    }
  }

  return SUCCESS;
}

int
php_cassandra_future_is_error(CassFuture* future TSRMLS_DC)
{
  int rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    CassString message = cass_future_error_message(future);
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
                            "%.*s", (int)message.length, message.data);
    return FAILURE;
  }
  return SUCCESS;
}
