#include "php_cassandra.h"
#include "util/result.h"
#include "util/ref.h"

zend_class_entry *cassandra_default_column_ce = NULL;

PHP_METHOD(DefaultColumn, name)
{
  cassandra_column* self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_column*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_ZVAL(self->name, 1, 0);
}

PHP_METHOD(DefaultColumn, type)
{
  cassandra_column* self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_column*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_ZVAL(self->type, 1, 0);
}

PHP_METHOD(DefaultColumn, isReversed)
{
  cassandra_column* self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_column*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_BOOL(self->reversed);
}

PHP_METHOD(DefaultColumn, isStatic)
{
  cassandra_column*          self;
  const CassSchemaMetaField* field;
  const CassValue*           value;
  const char*                str;
  size_t                     str_len;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self  = (cassandra_column*) zend_object_store_get_object(getThis() TSRMLS_CC);
  field = cass_schema_meta_get_field(self->meta, "type");
  value = cass_schema_meta_field_value(field);

  ASSERT_SUCCESS_BLOCK(cass_value_get_string(value, &str, &str_len),
    RETURN_FALSE;
  );

  if (strncmp("STATIC", str, str_len) == 0) {
    RETURN_TRUE;
  }

  RETURN_FALSE;
}

PHP_METHOD(DefaultColumn, isFrozen)
{
  cassandra_column* self;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_column*) zend_object_store_get_object(getThis() TSRMLS_CC);

  RETURN_BOOL(self->frozen);
}

PHP_METHOD(DefaultColumn, indexName)
{
  cassandra_column* self;
  zval* value;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_column*) zend_object_store_get_object(getThis() TSRMLS_CC);

  php_cassandra_get_schema_field(self->meta, "index_name", &value TSRMLS_CC);
  RETURN_ZVAL(value, 0, 1);
}

PHP_METHOD(DefaultColumn, indexOptions)
{
  cassandra_column* self;
  zval* value;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  self = (cassandra_column*) zend_object_store_get_object(getThis() TSRMLS_CC);

  php_cassandra_get_schema_field(self->meta, "index_options", &value TSRMLS_CC);
  RETURN_ZVAL(value, 0, 1);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_default_column_methods[] = {
  PHP_ME(DefaultColumn, name,         arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, type,         arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, isReversed,   arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, isStatic,     arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, isFrozen,     arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, indexName,    arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultColumn, indexOptions, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_default_column_handlers;

static HashTable*
php_cassandra_default_column_properties(zval *object TSRMLS_DC)
{
  HashTable* props = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_default_column_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_default_column_free(void *object TSRMLS_DC)
{
  cassandra_column* column = (cassandra_column*) object;

  zend_object_std_dtor(&column->zval TSRMLS_CC);

  if (column->name) {
    zval_ptr_dtor(&column->name);
    column->name = NULL;
  }

  if (column->type) {
    zval_ptr_dtor(&column->type);
    column->type = NULL;
  }

  if (column->schema) {
    php_cassandra_del_ref(&column->schema);
    column->schema = NULL;
  }
  column->meta = NULL;

  efree(column);
}

static zend_object_value
php_cassandra_default_column_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_column *column;

  column = (cassandra_column*) ecalloc(1, sizeof(cassandra_column));

  zend_object_std_init(&column->zval, class_type TSRMLS_CC);
  object_properties_init(&column->zval, class_type);

  column->name     = NULL;
  column->type     = NULL;
  column->reversed = 0;
  column->frozen   = 0;
  column->schema   = NULL;
  column->meta     = NULL;

  retval.handle   = zend_objects_store_put(column,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_default_column_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_default_column_handlers;

  return retval;
}

void cassandra_define_DefaultColumn(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\DefaultColumn", cassandra_default_column_methods);
  cassandra_default_column_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_default_column_ce TSRMLS_CC, 1, cassandra_column_ce);
  cassandra_default_column_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_default_column_ce->create_object = php_cassandra_default_column_new;

  memcpy(&cassandra_default_column_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_default_column_handlers.get_properties  = php_cassandra_default_column_properties;
  cassandra_default_column_handlers.compare_objects = php_cassandra_default_column_compare;
}
