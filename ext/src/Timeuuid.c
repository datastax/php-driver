/**
 * Copyright 2015-2016 DataStax, Inc.
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
#include "util/hash.h"
#include "util/types.h"
#include "util/uuid_gen.h"

#include <ext/date/php_date.h>

zend_class_entry *php_driver_timeuuid_ce = NULL;

void
php_driver_timeuuid_init(INTERNAL_FUNCTION_PARAMETERS)
{
  php_driver_uuid *self;
  zval *param;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &param) == FAILURE) {
    return;
  }

  if (getThis() && instanceof_function(Z_OBJCE_P(getThis()), php_driver_timeuuid_ce TSRMLS_CC)) {
    self = PHP_DRIVER_GET_UUID(getThis());
  } else {
    object_init_ex(return_value, php_driver_timeuuid_ce);
    self = PHP_DRIVER_GET_UUID(return_value);
  }


  if (ZEND_NUM_ARGS() == 0) {
    php_driver_uuid_generate_time(&self->uuid TSRMLS_CC);
  } else {

    switch (Z_TYPE_P(param)) {
      case IS_LONG:
        if (Z_LVAL_P(param) < 0) {
          zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Timestamp must be a positive integer, %d given", Z_LVAL_P(param));
          return;
        }
        php_driver_uuid_generate_from_time(Z_LVAL_P(param), &self->uuid TSRMLS_CC);
        break;
      case IS_STRING:
        if (cass_uuid_from_string(Z_STRVAL_P(param), &self->uuid) != CASS_OK) {
          zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid UUID: '%.*s'", Z_STRLEN_P(param), Z_STRVAL_P(param));
          return;
        }

        int version = cass_uuid_version(self->uuid);
        if (version != 1) {
          zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "UUID must be of type 1, type %d given", version);
        }
        break;
      default:
          zend_throw_exception_ex(php_driver_invalid_argument_exception_ce, 0 TSRMLS_CC, "Invalid argument - integer or string expected");
     }

  }
}

/* {{{ Timeuuid::__construct(string) */
PHP_METHOD(Timeuuid, __construct)
{
  php_driver_timeuuid_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Timeuuid::__toString() */
PHP_METHOD(Timeuuid, __toString)
{
  char string[CASS_UUID_STRING_LENGTH];
  php_driver_uuid *self = PHP_DRIVER_GET_UUID(getThis());

  cass_uuid_string(self->uuid, string);

  PHP5TO7_RETVAL_STRING(string);
}
/* }}} */

/* {{{ Timeuuid::type() */
PHP_METHOD(Timeuuid, type)
{
  php5to7_zval type = php_driver_type_scalar(CASS_VALUE_TYPE_TIMEUUID TSRMLS_CC);
  RETURN_ZVAL(PHP5TO7_ZVAL_MAYBE_P(type), 1, 1);
}
/* }}} */

/* {{{ Timeuuid::value() */
PHP_METHOD(Timeuuid, uuid)
{
  char string[CASS_UUID_STRING_LENGTH];
  php_driver_uuid *self = PHP_DRIVER_GET_UUID(getThis());

  cass_uuid_string(self->uuid, string);

  PHP5TO7_RETVAL_STRING(string);
}
/* }}} */

/* {{{ Timeuuid::version() */
PHP_METHOD(Timeuuid, version)
{
  php_driver_uuid *self = PHP_DRIVER_GET_UUID(getThis());

  RETURN_LONG((long) cass_uuid_version(self->uuid));
}
/* }}} */

/* {{{ Timeuuid::time() */
PHP_METHOD(Timeuuid, time)
{
  php_driver_uuid *self = PHP_DRIVER_GET_UUID(getThis());
  RETURN_LONG((long) (cass_uuid_timestamp(self->uuid) / 1000));
}
/* }}} */

/* {{{ Timeuuid::toDateTime() */
PHP_METHOD(Timeuuid, toDateTime)
{
  php_driver_uuid *self;
  zval datetime_object;
  zval *datetime = &datetime_object;
  php_date_obj *datetime_obj = NULL;
  char *str;
  int str_len;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_DRIVER_GET_UUID(getThis());

  PHP5TO7_ZVAL_MAYBE_MAKE(datetime);
  php_date_instantiate(php_date_get_date_ce(), datetime TSRMLS_CC);

#if PHP_MAJOR_VERSION >= 7
  datetime_obj = php_date_obj_from_obj(Z_OBJ_P(datetime));
#else
  datetime_obj = zend_object_store_get_object(datetime TSRMLS_CC);
#endif

  str_len      = spprintf(&str, 0, "@%ld", (long) (cass_uuid_timestamp(self->uuid) / 1000));
  php_date_initialize(datetime_obj, str, str_len, NULL, NULL, 0 TSRMLS_CC);
  efree(str);

  RETVAL_ZVAL(datetime, 0, 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo__construct, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry php_driver_timeuuid_methods[] = {
  PHP_ME(Timeuuid, __construct, arginfo__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, __toString, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, type, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, uuid, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, version, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, time, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(Timeuuid, toDateTime, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static php_driver_value_handlers php_driver_timeuuid_handlers;

static HashTable *
php_driver_timeuuid_gc(zval *object, php5to7_zval_gc table, int *n TSRMLS_DC)
{
  *table = NULL;
  *n = 0;
  return zend_std_get_properties(object TSRMLS_CC);
}

static HashTable *
php_driver_timeuuid_properties(zval *object TSRMLS_DC)
{
  char string[CASS_UUID_STRING_LENGTH];
  php5to7_zval type;
  php5to7_zval uuid;
  php5to7_zval version;

  php_driver_uuid *self = PHP_DRIVER_GET_UUID(object);
  HashTable      *props = zend_std_get_properties(object TSRMLS_CC);

  type = php_driver_type_scalar(CASS_VALUE_TYPE_TIMEUUID TSRMLS_CC);
  PHP5TO7_ZEND_HASH_UPDATE(props, "type", sizeof("type"), PHP5TO7_ZVAL_MAYBE_P(type), sizeof(zval));

  cass_uuid_string(self->uuid, string);

  PHP5TO7_ZVAL_MAYBE_MAKE(uuid);
  PHP5TO7_ZVAL_STRING(PHP5TO7_ZVAL_MAYBE_P(uuid), string);
  PHP5TO7_ZEND_HASH_UPDATE(props, "uuid", sizeof("uuid"), PHP5TO7_ZVAL_MAYBE_P(uuid), sizeof(zval));

  PHP5TO7_ZVAL_MAYBE_MAKE(version);
  ZVAL_LONG(PHP5TO7_ZVAL_MAYBE_P(version), (long) cass_uuid_version(self->uuid));
  PHP5TO7_ZEND_HASH_UPDATE(props, "version", sizeof("version"), PHP5TO7_ZVAL_MAYBE_P(version), sizeof(zval));

  return props;
}

static int
php_driver_timeuuid_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  php_driver_uuid *uuid1 = NULL;
  php_driver_uuid *uuid2 = NULL;

  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  uuid1 = PHP_DRIVER_GET_UUID(obj1);
  uuid2 = PHP_DRIVER_GET_UUID(obj2);

  if (uuid1->uuid.time_and_version != uuid2->uuid.time_and_version)
    return uuid1->uuid.time_and_version < uuid2->uuid.time_and_version ? -1 : 1;
  if (uuid1->uuid.clock_seq_and_node != uuid2->uuid.clock_seq_and_node)
    return uuid1->uuid.clock_seq_and_node < uuid2->uuid.clock_seq_and_node ? -1 : 1;

  return 0;
}

static unsigned
php_driver_timeuuid_hash_value(zval *obj TSRMLS_DC)
{
  php_driver_uuid *self = PHP_DRIVER_GET_UUID(obj);
  return php_driver_combine_hash(php_driver_bigint_hash(self->uuid.time_and_version),
                                    php_driver_bigint_hash(self->uuid.clock_seq_and_node));
}

static void
php_driver_timeuuid_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  php_driver_uuid *self = PHP5TO7_ZEND_OBJECT_GET(uuid, object);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_driver_timeuuid_new(zend_class_entry *ce TSRMLS_DC)
{
  php_driver_uuid *self =
      PHP5TO7_ZEND_OBJECT_ECALLOC(uuid, ce);

  PHP5TO7_ZEND_OBJECT_INIT_EX(uuid, timeuuid, self, ce);
}

void
php_driver_define_Timeuuid(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, PHP_DRIVER_NAMESPACE "\\Timeuuid", php_driver_timeuuid_methods);
  php_driver_timeuuid_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(php_driver_timeuuid_ce TSRMLS_CC, 2, php_driver_value_ce, php_driver_uuid_interface_ce);
  memcpy(&php_driver_timeuuid_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  php_driver_timeuuid_handlers.std.get_properties  = php_driver_timeuuid_properties;
#if PHP_VERSION_ID >= 50400
  php_driver_timeuuid_handlers.std.get_gc          = php_driver_timeuuid_gc;
#endif
  php_driver_timeuuid_handlers.std.compare_objects = php_driver_timeuuid_compare;
  php_driver_timeuuid_ce->ce_flags |= PHP5TO7_ZEND_ACC_FINAL;
  php_driver_timeuuid_ce->create_object = php_driver_timeuuid_new;

  php_driver_timeuuid_handlers.hash_value = php_driver_timeuuid_hash_value;
  php_driver_timeuuid_handlers.std.clone_obj = NULL;
}
