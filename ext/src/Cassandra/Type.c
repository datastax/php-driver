#include "php_cassandra.h"
#include "util/types.h"

zend_class_entry *cassandra_type_ce = NULL;

#define XX_SCALAR_METHOD(name, value) PHP_METHOD(Type, name) \
{ \
  zval* ztype; \
  if (zend_parse_parameters_none() == FAILURE) { \
    return; \
  } \
  ztype = php_cassandra_type_scalar(value TSRMLS_CC); \
  RETURN_ZVAL(ztype, 1, 0); \
}

PHP_CASSANDRA_SCALAR_TYPES_MAP(XX_SCALAR_METHOD)
#undef XX_SCALAR_METHOD

PHP_METHOD(Type, collection)
{
  zval *ztype;
  zval *type;
  cassandra_type_scalar* scalar;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O",
                            &type, cassandra_type_ce) == FAILURE) {
    return;
  }

  if (!instanceof_function(Z_OBJCE_P(type),
                           cassandra_type_scalar_ce TSRMLS_CC)) {
    throw_invalid_argument(type, "type", "Cassandra\\Type::varchar(), " \
                                         "Cassandra\\Type::text(), " \
                                         "Cassandra\\Type::blob(), " \
                                         "Cassandra\\Type::ascii(), " \
                                         "Cassandra\\Type::bigint(), " \
                                         "Cassandra\\Type::counter(), " \
                                         "Cassandra\\Type::int(), " \
                                         "Cassandra\\Type::varint(), " \
                                         "Cassandra\\Type::boolean(), " \
                                         "Cassandra\\Type::decimal(), " \
                                         "Cassandra\\Type::double(), " \
                                         "Cassandra\\Type::float(), " \
                                         "Cassandra\\Type::inet(), " \
                                         "Cassandra\\Type::timestamp(), " \
                                         "Cassandra\\Type::uuid() or " \
                                         "Cassandra\\Type::timeuuid()" TSRMLS_CC);
    return;
  }

  scalar = (cassandra_type_scalar*) zend_object_store_get_object(type TSRMLS_CC);
  ztype  = php_cassandra_type_collection(scalar->type TSRMLS_CC);
  RETURN_ZVAL(ztype, 0, 1);
}

PHP_METHOD(Type, set)
{
  zval* ztype;
  zval *type;
  cassandra_type_scalar* scalar;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O",
                            &type, cassandra_type_ce) == FAILURE) {
    return;
  }

  if (!instanceof_function(Z_OBJCE_P(type),
                           cassandra_type_scalar_ce TSRMLS_CC)) {
    throw_invalid_argument(type, "type", "Cassandra\\Type::varchar(), " \
                                         "Cassandra\\Type::text(), " \
                                         "Cassandra\\Type::blob(), " \
                                         "Cassandra\\Type::ascii(), " \
                                         "Cassandra\\Type::bigint(), " \
                                         "Cassandra\\Type::counter(), " \
                                         "Cassandra\\Type::int(), " \
                                         "Cassandra\\Type::varint(), " \
                                         "Cassandra\\Type::boolean(), " \
                                         "Cassandra\\Type::decimal(), " \
                                         "Cassandra\\Type::double(), " \
                                         "Cassandra\\Type::float(), " \
                                         "Cassandra\\Type::inet(), " \
                                         "Cassandra\\Type::timestamp(), " \
                                         "Cassandra\\Type::uuid() or " \
                                         "Cassandra\\Type::timeuuid()" TSRMLS_CC);
    return;
  }

  scalar = (cassandra_type_scalar*) zend_object_store_get_object(type TSRMLS_CC);
  ztype  = php_cassandra_type_set(scalar->type TSRMLS_CC);
  RETURN_ZVAL(ztype, 0, 1);
}

PHP_METHOD(Type, map)
{
  zval* ztype;
  zval *key_type;
  cassandra_type_scalar* key_scalar;
  zval *value_type;
  cassandra_type_scalar* value_scalar;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OO",
                            &key_type, cassandra_type_ce,
                            &value_type, cassandra_type_ce) == FAILURE) {
    return;
  }

  if (!instanceof_function(Z_OBJCE_P(key_type),
                           cassandra_type_scalar_ce TSRMLS_CC)) {
    throw_invalid_argument(key_type, "keyType",
                           "Cassandra\\Type::varchar(), " \
                           "Cassandra\\Type::text(), " \
                           "Cassandra\\Type::blob(), " \
                           "Cassandra\\Type::ascii(), " \
                           "Cassandra\\Type::bigint(), " \
                           "Cassandra\\Type::counter(), " \
                           "Cassandra\\Type::int(), " \
                           "Cassandra\\Type::varint(), " \
                           "Cassandra\\Type::boolean(), " \
                           "Cassandra\\Type::decimal(), " \
                           "Cassandra\\Type::double(), " \
                           "Cassandra\\Type::float(), " \
                           "Cassandra\\Type::inet(), " \
                           "Cassandra\\Type::timestamp(), " \
                           "Cassandra\\Type::uuid() or " \
                           "Cassandra\\Type::timeuuid()" TSRMLS_CC);
    return;
  }

  if (!instanceof_function(Z_OBJCE_P(value_type),
                           cassandra_type_scalar_ce TSRMLS_CC)) {
    throw_invalid_argument(value_type, "valueType",
                           "Cassandra\\Type::varchar(), " \
                           "Cassandra\\Type::text(), " \
                           "Cassandra\\Type::blob(), " \
                           "Cassandra\\Type::ascii(), " \
                           "Cassandra\\Type::bigint(), " \
                           "Cassandra\\Type::counter(), " \
                           "Cassandra\\Type::int(), " \
                           "Cassandra\\Type::varint(), " \
                           "Cassandra\\Type::boolean(), " \
                           "Cassandra\\Type::decimal(), " \
                           "Cassandra\\Type::double(), " \
                           "Cassandra\\Type::float(), " \
                           "Cassandra\\Type::inet(), " \
                           "Cassandra\\Type::timestamp(), " \
                           "Cassandra\\Type::uuid() or " \
                           "Cassandra\\Type::timeuuid()" TSRMLS_CC);
    return;
  }

  key_scalar   = (cassandra_type_scalar*) zend_object_store_get_object(key_type TSRMLS_CC);
  value_scalar = (cassandra_type_scalar*) zend_object_store_get_object(value_type TSRMLS_CC);
  ztype        = php_cassandra_type_map(key_scalar->type, value_scalar->type TSRMLS_CC);
  RETURN_ZVAL(ztype, 0, 1);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_value, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_type, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, type, Cassandra\\Type, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_map, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_OBJ_INFO(0, keyType,   Cassandra\\Type, 0)
  ZEND_ARG_OBJ_INFO(0, valueType, Cassandra\\Type, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_type_methods[] = {
  PHP_ABSTRACT_ME(Type, name,       arginfo_none)
  PHP_ABSTRACT_ME(Type, __toString, arginfo_none)
  PHP_ABSTRACT_ME(Type, create,     arginfo_value)

#define XX_SCALAR_METHOD(name, _) PHP_ME(Type, name, arginfo_none, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
  PHP_CASSANDRA_SCALAR_TYPES_MAP(XX_SCALAR_METHOD)
#undef XX_SCALAR_METHOD
  PHP_ME(Type, collection, arginfo_type, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
  PHP_ME(Type, set,        arginfo_type, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)
  PHP_ME(Type, map,        arginfo_map,  ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_FINAL)

  PHP_FE_END
};

void cassandra_define_Type(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Type", cassandra_type_methods);
  cassandra_type_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_type_ce->ce_flags |= ZEND_ACC_INTERFACE;
}
