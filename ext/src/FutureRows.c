/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "php_driver.h"
#include "php_driver_types.h"
#include "util/future.h"
#include "util/result.h"
#include "util/ref.h"

zend_class_entry *php_driver_future_rows_ce = NULL;

static void
free_result(void *result)
{
  cass_result_free((CassResult *) result);
}

int
php_driver_future_rows_get_result(php_driver_future_rows *future_rows, zval *timeout TSRMLS_DC)
{
  if (!future_rows->result) {
    const CassResult *result = NULL;

    if (php_driver_future_wait_timed(future_rows->future, timeout TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }

    if (php_driver_future_is_error(future_rows->future TSRMLS_CC) == FAILURE) {
      return FAILURE;
    }

    result = cass_future_get_result(future_rows->future);
    if (!result) {
      zend_throw_exception_ex(php_driver_runtime_exception_ce, 0 TSRMLS_CC,
                              "Future doesn't contain a result.");
      return FAILURE;
    }

    future_rows->result = php_driver_new_ref((void *)result, free_result);
  }

  return SUCCESS;
}

PHP_METHOD(FutureRows, get)
{
  zval *timeout = NULL;
  php_driver_rows *rows = NULL;

  php_driver_future_rows *self = PHP_DRIVER_GET_FUTURE_ROWS(getThis());

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &timeout) == FAILURE) {
    return;
  }

  if (php_driver_future_rows_get_result(self, timeout TSRMLS_CC) == FAILURE) {
    return;
  }

  if (PHP5TO7_ZVAL_IS_UNDEF(self->rows)) {
    if (php_driver_get_result((const CassResult *) self->result->data,
                                 &self->rows TSRMLS_CC) == FAILURE) {
      PHP5TO7_ZVAL_MAYBE_DESTROY(self->rows);
      return;
    }
  }

  object_init_ex(return_value, php_driver_rows_ce);
  rows = PHP_DRIVER_GET_ROWS(return_value);

  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(rows->rows),
                    PHP5TO7_ZVAL_MAYBE_P(self->rows));

  if (cass_result_has_more_pages((const CassResult *)self->result->data)) {
    rows->session   = php_driver_add_ref(self->session);
    rows->statement = php_driver_add_ref(self->statement);
    rows->result    = php_driver_add_ref(self->result);
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_future_rows_methods[] = {
  PHP_ME(FutureRows, get, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers php_driver_future_rows_handlers;

static HashTable *
php_driver_future_rows_properties(php7to8_object *object TSRMLS_DC)
{
  HashTable *props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_driver_future_rows_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  PHP7TO8_MAYBE_COMPARE_OBJECTS_FALLBACK(obj1, obj2);
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_driver_future_rows_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_future_rows *self = PHP5TO7_ZEND_OBJECT_GET(future_rows, object);

  PHP5TO7_ZVAL_MAYBE_DESTROY(self->rows);

  php_driver_del_ref(&self->statement);
  php_driver_del_peref(&self->session, 1);
  php_driver_del_ref(&self->result);

  if (self->future) {
    cass_future_free(self->future);
  }

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_future_rows_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_future_rows *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(future_rows, ce);

  self->future    = NULL;
  self->statement = NULL;
  self->result    = NULL;
  self->session   = NULL;
  PHP5TO7_ZVAL_UNDEF(self->rows);

  PHP5TO7_ZEND_OBJECT_INIT(future_rows, self, ce);
}

void php_driver_define_FutureRows(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\FutureRows", php_driver_future_rows_methods);
  php_driver_future_rows_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_future_rows_ce TSRMLS_CC, 1, php_driver_future_ce);
  php_driver_future_rows_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_future_rows_ce->create_object = php_driver_future_rows_new;

  memcpy(&php_driver_future_rows_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_future_rows_handlers.get_properties  = php_driver_future_rows_properties;
  PHP7TO8_COMPARE(php_driver_future_rows_handlers, php_driver_future_rows_compare);
  php_driver_future_rows_handlers.clone_obj = NULL;
}
