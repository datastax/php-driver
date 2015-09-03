#include "php_cassandra.h"
#include "util/types.h"
#include <ext/standard/php_smart_str.h>
#include "src/Cassandra/Bigint.h"
#include "src/Cassandra/Blob.h"
#include "src/Cassandra/Decimal.h"
#include "src/Cassandra/Float.h"
#include "src/Cassandra/Inet.h"
#include "src/Cassandra/Timestamp.h"
#include "src/Cassandra/Timeuuid.h"
#include "src/Cassandra/Uuid.h"
#include "src/Cassandra/Varint.h"

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

static zval*
php_cassandra_type_scalar_new(CassValueType type TSRMLS_DC)
{
  zval* ztype;
  cassandra_type_scalar* scalar;

  MAKE_STD_ZVAL(ztype);
  object_init_ex(ztype, cassandra_type_scalar_ce);
  scalar = (cassandra_type_scalar*) zend_object_store_get_object(ztype TSRMLS_CC);
  scalar->type = type;

  return ztype;
}


const char*
php_cassandra_scalar_type_name(CassValueType type TSRMLS_DC)
{
  switch (type) {
#define XX_SCALAR(name, value) \
  case value: \
    return #name;
  PHP_CASSANDRA_SCALAR_TYPES_MAP(XX_SCALAR)
#undef XX_SCALAR
  default:
    return "invalid";
  }
}

static void
php_cassandra_varchar_init(INTERNAL_FUNCTION_PARAMETERS)
{
  char *string;
  int string_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
    return;
  }

  RETURN_STRINGL(string, string_len, 1);
}

static void
php_cassandra_ascii_init(INTERNAL_FUNCTION_PARAMETERS)
{
  php_cassandra_varchar_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static void
php_cassandra_boolean_init(INTERNAL_FUNCTION_PARAMETERS)
{
  zend_bool value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &value) == FAILURE) {
    return;
  }

  RETURN_BOOL(value);
}

static void
php_cassandra_counter_init(INTERNAL_FUNCTION_PARAMETERS)
{
  php_cassandra_bigint_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static void
php_cassandra_double_init(INTERNAL_FUNCTION_PARAMETERS)
{
  double value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &value) == FAILURE) {
    return;
  }

  RETURN_DOUBLE(value);
}

static void
php_cassandra_int_init(INTERNAL_FUNCTION_PARAMETERS)
{
  long value;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
    return;
  }

  RETURN_LONG(value);
}

static void
php_cassandra_text_init(INTERNAL_FUNCTION_PARAMETERS)
{
  php_cassandra_varchar_init(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

#define TYPE_INIT_METHOD(t) php_cassandra_ ## t ## _init

void
php_cassandra_scalar_init(INTERNAL_FUNCTION_PARAMETERS)
{
  cassandra_type_scalar* self;

  self = (cassandra_type_scalar*) zend_object_store_get_object(getThis() TSRMLS_CC);

#define XX_SCALAR(name, value) \
  if (self->type == value) { \
    TYPE_INIT_METHOD(name)(INTERNAL_FUNCTION_PARAM_PASSTHRU); \
  }
  PHP_CASSANDRA_SCALAR_TYPES_MAP(XX_SCALAR)
#undef XX_SCALAR
}
#undef TYPE_INIT_METHOD

#define TYPE_CODE(m) type_ ## m

zval*
php_cassandra_type_scalar(CassValueType type TSRMLS_DC)
{
#define XX_SCALAR(name, value) \
  if (value == type) { \
    if (CASSANDRA_G(TYPE_CODE(name)) == NULL) { \
      CASSANDRA_G(TYPE_CODE(name)) = php_cassandra_type_scalar_new(type TSRMLS_CC); \
    } \
    return CASSANDRA_G(TYPE_CODE(name)); \
  }
  PHP_CASSANDRA_SCALAR_TYPES_MAP(XX_SCALAR)
#undef XX_SCALAR

  zend_throw_exception_ex(cassandra_invalid_argument_exception_ce,
                          0 TSRMLS_CC, "Invalid type");
  return NULL;
}
#undef TYPE_CODE

zval*
php_cassandra_type_map(CassValueType key_type,
                       CassValueType value_type TSRMLS_DC)
{
  zval* ztype;
  cassandra_type_map* map;

  MAKE_STD_ZVAL(ztype);
  object_init_ex(ztype, cassandra_type_map_ce);
  map = (cassandra_type_map*) zend_object_store_get_object(ztype TSRMLS_CC);
  map->key_type   = key_type;
  map->value_type = value_type;

  return ztype;
}

zval*
php_cassandra_type_set(CassValueType type TSRMLS_DC)
{
  zval* ztype;
  cassandra_type_set* set;

  MAKE_STD_ZVAL(ztype);
  object_init_ex(ztype, cassandra_type_set_ce);
  set = (cassandra_type_set*) zend_object_store_get_object(ztype TSRMLS_CC);
  set->type = type;

  return ztype;
}

zval*
php_cassandra_type_collection(CassValueType type TSRMLS_DC)
{
  zval* ztype;
  cassandra_type_collection* collection;

  MAKE_STD_ZVAL(ztype);
  object_init_ex(ztype, cassandra_type_collection_ce);
  collection = (cassandra_type_collection*) zend_object_store_get_object(ztype TSRMLS_CC);
  collection->type = type;

  return ztype;
}

zval*
php_cassandra_type_custom(char* name TSRMLS_DC)
{
  zval* ztype;
  cassandra_type_custom* custom;

  MAKE_STD_ZVAL(ztype);
  object_init_ex(ztype, cassandra_type_custom_ce);
  custom = (cassandra_type_custom*) zend_object_store_get_object(ztype TSRMLS_CC);
  custom->name = name;

  return ztype;
}

#define EXPECTING_TOKEN(expected) \
  zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC, \
    "Unexpected %s at position %d in string \"%s\", expected " expected, \
    describe_token(token), ((int) (str - validator) - 1), validator \
  ); \
  return FAILURE;

enum token_type {
  TOKEN_ILLEGAL = 0,
  TOKEN_PAREN_OPEN,
  TOKEN_PAREN_CLOSE,
  TOKEN_COMMA,
  TOKEN_COLON,
  TOKEN_NAME,
  TOKEN_END
};

enum parser_state {
  STATE_CLASS = 0,
  STATE_AFTER_CLASS,
  STATE_AFTER_PARENS,
  STATE_END
};

static const char*
describe_token(enum token_type token)
{
  switch (token) {
  case TOKEN_ILLEGAL:     return "illegal character";      break;
  case TOKEN_PAREN_OPEN:  return "opening parenthesis";    break;
  case TOKEN_PAREN_CLOSE: return "closing parenthesis";    break;
  case TOKEN_COMMA:       return "comma";                  break;
  case TOKEN_COLON:       return "colon";                  break;
  case TOKEN_NAME:        return "alphanumeric character"; break;
  case TOKEN_END:         return "end of string";          break;
  default:                return "unknown token";
  }
}

static int
isletter(char ch)
{
  return isalnum(ch) || ch == '.';
}

static enum token_type
next_token(const char*  str,       size_t  len,
           const char** token_str, size_t* token_len,
           const char** str_out,   size_t* len_out)
{
  enum token_type type;

  unsigned int i = 0;
  char         c = str[i];

  if (isalpha(c)) {
    type = TOKEN_NAME;
    while (i < len) {
      if (!isletter(str[i])) {
        break;
      }
      i++;
    }
  } else {
    switch (c) {
      case '\0':
        type = TOKEN_END;
        break;
      case '(':
        type = TOKEN_PAREN_OPEN;
        i++;
        break;
      case ')':
        type = TOKEN_PAREN_CLOSE;
        i++;
        break;
      case ',':
        type = TOKEN_COMMA;
        i++;
        break;
      case ':':
        type = TOKEN_COLON;
        i++;
        break;
      default:
        type = TOKEN_ILLEGAL;
    }
  }

  *token_str = &(str[0]);
  *token_len = i;
  *str_out   = &(str[i]);
  *len_out   = len - i;

  return type;
}

struct node_s {
  struct node_s* parent;
  const char*    name;
  size_t         name_length;
  struct node_s* first_child;
  struct node_s* last_child;
  struct node_s* next_sibling;
  struct node_s* prev_sibling;
};

static struct node_s*
php_cassandra_parse_node_new()
{
  struct node_s* node;
  node = emalloc(sizeof(struct node_s));
  node->parent        = NULL;
  node->name          = NULL;
  node->name_length   = 0;
  node->first_child   = NULL;
  node->last_child    = NULL;
  node->next_sibling  = NULL;
  node->prev_sibling  = NULL;

  return node;
}

static void
php_cassandra_parse_node_free(struct node_s* node)
{
  if (node->first_child) {
    php_cassandra_parse_node_free(node->first_child);
    node->first_child = NULL;
  }
  node->last_child = NULL;

  if (node->next_sibling) {
    php_cassandra_parse_node_free(node->next_sibling);
    node->next_sibling = NULL;
  }

  efree(node);
}

static int
php_cassandra_parse_class_name(const char*     validator,
                               size_t          validator_len,
                               struct node_s** result TSRMLS_DC)
{
  const char*       str;
  size_t            len;
  const char*       token_str;
  size_t            token_len;
  enum parser_state state;
  enum token_type   token;
  struct node_s*    root;
  struct node_s*    node;
  struct node_s*    child;

  token_str = NULL;
  token_len = 0;
  state     = STATE_CLASS;
  str       = validator;
  len       = validator_len;
  root      = php_cassandra_parse_node_new();
  node      = root;

  while (1) {
    token = next_token(str, len, &token_str, &token_len, &str, &len);

    if (token == TOKEN_ILLEGAL) {
      zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
        "Illegal character \"%c\" at position %d in \"%s\"",
        *token_str, ((int) (str - validator) - 1), validator);
      php_cassandra_parse_node_free(root);
      return FAILURE;
    }

    if (state == STATE_AFTER_PARENS) {
      if (token == TOKEN_COMMA) {
        if (node->parent == NULL) {
          EXPECTING_TOKEN("end of string");
        }

        node = node->parent;
        state = STATE_CLASS;
        continue;
      } else if (token == TOKEN_PAREN_CLOSE) {
        if (node->parent == NULL) {
          EXPECTING_TOKEN("end of string");
        }

        node = node->parent;
        continue;
      } else if (token == TOKEN_END) {
        break;
      } else {
        EXPECTING_TOKEN("a coma, a closing parenthesis or an end of string");
      }
    }

    if (state == STATE_AFTER_CLASS) {
      if (token == TOKEN_PAREN_OPEN) {
        state = STATE_CLASS;

        child = php_cassandra_parse_node_new();
        child->parent = node;

        if (root->first_child == NULL) {
          root->first_child = child;
        }

        if (node->last_child) {
          node->last_child->next_sibling = child;
        }

        child->prev_sibling = node->last_child;
        node->last_child = child;

        node = child;
        continue;
      } else if (token == TOKEN_COMMA) {
        state = STATE_CLASS;

        child = php_cassandra_parse_node_new();
        child->parent        = node->parent;
        child->prev_sibling  = node;
        node->next_sibling   = child;
        node->parent->last_child = child;

        node = child;
        continue;
      } else if (token == TOKEN_PAREN_CLOSE) {
        state = STATE_AFTER_PARENS;
        continue;
      } else if (token == TOKEN_END) {
        break;
      } else {
        php_cassandra_parse_node_free(root);
        EXPECTING_TOKEN("opening/closing parenthesis or coma");
      }
    }

    if (state == STATE_CLASS) {
      if (token != TOKEN_NAME) {
        php_cassandra_parse_node_free(root);
        EXPECTING_TOKEN("fully qualified class name");
      }

      node->name        = token_str;
      node->name_length = token_len;
      state = STATE_AFTER_CLASS;
    }
  }

  *result = root;
  return SUCCESS;
}

static CassValueType
php_cassandra_lookup_type(struct node_s* node TSRMLS_DC)
{

  if (strncmp("org.apache.cassandra.db.marshal.FrozenType", node->name, node->name_length)    == 0 ||
      strncmp("org.apache.cassandra.db.marshal.ReversedType", node->name, node->name_length)  == 0 ||
      strncmp("org.apache.cassandra.db.marshal.CompositeType", node->name, node->name_length) == 0) {
    return php_cassandra_lookup_type(node->first_child TSRMLS_CC);
  }

  if (strncmp("org.apache.cassandra.db.marshal.AsciiType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_ASCII;
  }

  if (strncmp("org.apache.cassandra.db.marshal.LongType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_BIGINT;
  }

  if (strncmp("org.apache.cassandra.db.marshal.BytesType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_BLOB;
  }

  if (strncmp("org.apache.cassandra.db.marshal.BooleanType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_BOOLEAN;
  }

  if (strncmp("org.apache.cassandra.db.marshal.CounterColumnType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_COUNTER;
  }

  if (strncmp("org.apache.cassandra.db.marshal.DecimalType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_DECIMAL;
  }

  if (strncmp("org.apache.cassandra.db.marshal.DoubleType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_DOUBLE;
  }

  if (strncmp("org.apache.cassandra.db.marshal.FloatType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_FLOAT;
  }

  if (strncmp("org.apache.cassandra.db.marshal.InetAddressType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_INET;
  }

  if (strncmp("org.apache.cassandra.db.marshal.Int32Type", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_INT;
  }

  if (strncmp("org.apache.cassandra.db.marshal.UTF8Type", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_VARCHAR;
  }

  if (strncmp("org.apache.cassandra.db.marshal.TimestampType", node->name, node->name_length) == 0 ||
      strncmp("org.apache.cassandra.db.marshal.DateType",      node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_TIMESTAMP;
  }

  if (strncmp("org.apache.cassandra.db.marshal.UUIDType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_UUID;
  }

  if (strncmp("org.apache.cassandra.db.marshal.IntegerType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_VARINT;
  }

  if (strncmp("org.apache.cassandra.db.marshal.TimeUUIDType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_TIMEUUID;
  }

  if (strncmp("org.apache.cassandra.db.marshal.MapType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_MAP;
  }

  if (strncmp("org.apache.cassandra.db.marshal.SetType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_SET;
  }

  if (strncmp("org.apache.cassandra.db.marshal.ListType", node->name, node->name_length) == 0) {
    return CASS_VALUE_TYPE_LIST;
  }

  return CASS_VALUE_TYPE_CUSTOM;
}

static void
php_cassandra_node_dump_to(struct node_s* node, smart_str* text)
{
  smart_str_appendl(text, node->name, node->name_length);

  if (node->first_child) {
    smart_str_appendl(text, "(", 1);
    php_cassandra_node_dump_to(node->first_child, text);
    smart_str_appendl(text, ")", 1);
  }

  if (node->next_sibling) {
    smart_str_appendl(text, ", ", 2);
    php_cassandra_node_dump_to(node->next_sibling, text);
  }
}

static char*
php_cassandra_node_dump(struct node_s* node)
{
  smart_str text = {NULL, 0, 0};

  php_cassandra_node_dump_to(node, &text);
  smart_str_0(&text);

  return text.c;
}

static int
php_cassandra_create_type(struct node_s* node, zval** type_out TSRMLS_DC)
{
  CassValueType type = php_cassandra_lookup_type(node TSRMLS_CC);

  if (type == CASS_VALUE_TYPE_UNKNOWN) {
    return FAILURE;
  }

  if (type == CASS_VALUE_TYPE_CUSTOM) {
    *type_out = php_cassandra_type_custom(php_cassandra_node_dump(node) TSRMLS_CC);
  } else if (type == CASS_VALUE_TYPE_MAP) {
    *type_out = php_cassandra_type_map(php_cassandra_lookup_type(node->first_child TSRMLS_CC),
                                       php_cassandra_lookup_type(node->first_child->next_sibling TSRMLS_CC) TSRMLS_CC);
  } else if (type == CASS_VALUE_TYPE_LIST) {
    *type_out = php_cassandra_type_collection(php_cassandra_lookup_type(node->first_child TSRMLS_CC) TSRMLS_CC);
  } else if (type == CASS_VALUE_TYPE_SET) {
    *type_out = php_cassandra_type_set(php_cassandra_lookup_type(node->first_child TSRMLS_CC) TSRMLS_CC);
  } else {
    *type_out = php_cassandra_type_scalar(type TSRMLS_CC);
    Z_ADDREF_P(*type_out);
  }

  return SUCCESS;
}

int
php_cassandra_parse_column_type(const char*  validator,
                                size_t       validator_len,
                                int*         reversed_out,
                                int*         frozen_out,
                                zval**       type_out TSRMLS_DC)
{
  struct node_s* root;
  struct node_s* node  = NULL;
  cass_bool_t reversed = 0;
  cass_bool_t frozen   = 0;

  if (php_cassandra_parse_class_name(validator, validator_len, &root TSRMLS_CC) == FAILURE) {
    return FAILURE;
  }

  node = root;

  while (node) {
    if (strncmp("org.apache.cassandra.db.marshal.ReversedType", node->name, node->name_length) == 0) {
      reversed = 1;
      node     = node->first_child;
      continue;
    }

    if (strncmp("org.apache.cassandra.db.marshal.FrozenType", node->name, node->name_length) == 0) {
      frozen = 1;
      node   = node->first_child;
      continue;
    }

    if (strncmp("org.apache.cassandra.db.marshal.CompositeType", node->name, node->name_length) == 0) {
      node = node->first_child;
      continue;
    }

    break;
  }

  if (node == NULL) {
    php_cassandra_parse_node_free(root);
    zend_throw_exception_ex(cassandra_invalid_argument_exception_ce, 0 TSRMLS_CC,
      "Invalid type");
    return FAILURE;
  }

  php_cassandra_create_type(node, type_out TSRMLS_CC);
  php_cassandra_parse_node_free(root);

  *reversed_out = reversed;
  *frozen_out   = frozen;

  return SUCCESS;
}
