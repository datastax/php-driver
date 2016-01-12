#include "php_cassandra.h"
#include "util/types.h"
#include "src/Cassandra/Udt.h"
#include "util/collections.h"
#if PHP_MAJOR_VERSION >= 7
#include <zend_smart_str.h>
#else
#include <ext/standard/php_smart_str.h>
#endif

zend_class_entry *cassandra_type_udt_ce = NULL;

int php_cassandra_type_udt_add(cassandra_type *type,
                               const char *name, size_t name_length,
                               zval *zsub_type TSRMLS_DC)
{
  cassandra_type *sub_type = PHP_CASSANDRA_GET_TYPE(zsub_type);
  if (cass_data_type_add_sub_type_by_name_n(type->data_type,
                                            name, name_length,
                                            sub_type->data_type) != CASS_OK) {
    return 0;
  }
  PHP5TO7_ZEND_HASH_ADD(&type->types,
                        name, name_length + 1,
                        zsub_type, sizeof(zval *));
  return 1;
}

PHP_METHOD(TypeUdt, name)
{
  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  PHP5TO7_RETVAL_STRING("udt");
}

PHP_METHOD(TypeUdt, types)
{
  cassandra_type *self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE(getThis());

  array_init(return_value);
  PHP5TO7_ZEND_HASH_ZVAL_COPY(Z_ARRVAL_P(return_value), &self->types);
}

PHP_METHOD(TypeUdt, __toString)
{
  cassandra_type *self;
  smart_str string = PHP5TO7_SMART_STR_INIT;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE(getThis());

  php_cassandra_type_string(self, &string TSRMLS_CC);
  smart_str_0(&string);

  PHP5TO7_RETVAL_STRING(PHP5TO7_SMART_STR_VAL(string));
  smart_str_free(&string);
}

PHP_METHOD(TypeUdt, create)
{
  cassandra_type *self;
  cassandra_udt *udt;
  php5to7_zval_args args = NULL;
  int argc = 0, i;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "*",
                            &args, &argc) == FAILURE) {
    return;
  }

  self = PHP_CASSANDRA_GET_TYPE(getThis());

  object_init_ex(return_value, cassandra_udt_ce);
  udt = PHP_CASSANDRA_GET_UDT(return_value);

  PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(udt->type), getThis());

  if (argc > 0) {
    if (argc % 2 == 1) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                              "Not enough name/value pairs, udts can only be created " \
                              "from an even number of name/value pairs, where each odd " \
                              "argument is a name and each even argument is a value, " \
                              "e.g udt(name, value, name, value, name, value)");
      PHP5TO7_MAYBE_EFREE(args);
      return;
    }

    if ((argc / 2) != zend_hash_num_elements(&self->types)) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce,
                              0 TSRMLS_CC,
                              "Invalid number of elements given. Expected %d arguments.",
                              zend_hash_num_elements(&self->types));
      PHP5TO7_MAYBE_EFREE(args);
      return;
    }

    for (i = 0; i < argc; i += 2) {
      zval *name = PHP5TO7_ZVAL_ARG(args[i]);
      zval *value = PHP5TO7_ZVAL_ARG(args[i + 1]);
      php5to7_zval *sub_type;
      if (Z_TYPE_P(name) != IS_STRING) {
        zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
                                "Argument %d is not a string", i + 1);
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
      if (!PHP5TO7_ZEND_HASH_FIND(&self->types,
                                  Z_STRVAL_P(name), Z_STRLEN_P(name) + 1,
                                  sub_type)) {
        zend_throw_exception_ex(cassandra_invalid_argument_exception_ce,
                                0 TSRMLS_CC,
                                "Invalid name '%s'", Z_STRVAL_P(name));
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
      if (!php_cassandra_validate_object(value,
                                         PHP5TO7_ZVAL_MAYBE_DEREF(sub_type) TSRMLS_CC)) {
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
      if (!php_cassandra_udt_set(udt,
                                 Z_STRVAL_P(name), Z_STRLEN_P(name),
                                 value TSRMLS_CC)) {
        PHP5TO7_MAYBE_EFREE(args);
        return;
      }
    }

    PHP5TO7_MAYBE_EFREE(args);
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_type_udt_methods[] = {
  PHP_ME(TypeUdt, name,       arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeUdt, __toString, arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeUdt, types,      arginfo_none,  ZEND_ACC_PUBLIC)
  PHP_ME(TypeUdt, create,     arginfo_value, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_type_udt_handlers;

static void
php_cassandra_type_udt_free(php5to7_zend_object_free *object TSRMLS_DC)
{
  cassandra_type *self = PHP5TO7_ZEND_OBJECT_GET(type, object);

  if (self->data_type) cass_data_type_free(self->data_type);
  if (self->keyspace) efree(self->keyspace);
  if (self->type_name) efree(self->type_name);
  zend_hash_destroy(&self->types);

  zend_object_std_dtor(&self->zval TSRMLS_CC);
  PHP5TO7_MAYBE_EFREE(self);
}

static php5to7_zend_object
php_cassandra_type_udt_new(zend_class_entry *ce TSRMLS_DC)
{
  cassandra_type *self = PHP5TO7_ZEND_OBJECT_ECALLOC(type, ce);

  self->type = CASS_VALUE_TYPE_UDT;
  self->data_type = cass_data_type_new(self->type);
  self->keyspace = self->type_name = NULL;
  zend_hash_init(&self->types, 0, NULL, ZVAL_PTR_DTOR, 0);

  PHP5TO7_ZEND_OBJECT_INIT_EX(type, type_udt, self, ce);
}

void cassandra_define_TypeUdt(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type\\Udt", cassandra_type_udt_methods);
  cassandra_type_udt_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_type_udt_ce TSRMLS_CC, 1, cassandra_type_ce);
  cassandra_type_udt_ce->ce_flags     |= PHP5TO7_ZEND_ACC_FINAL;
  cassandra_type_udt_ce->create_object = php_cassandra_type_udt_new;

  memcpy(&cassandra_type_udt_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
}
