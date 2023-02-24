#include <php.h>
#include <string>

#include "String.h"

namespace Zend
{
String::String(const char *other, size_t len) noexcept
    : str(zend_string_init(other, len, false)) {}

String::String(const char *other) noexcept
    : str(zend_string_init(other, strlen(other), false)) {}

String::String(const zend_string *other) noexcept
    : str(zend_string_copy((zend_string *)other)) {}

String::String(zend_string *other) noexcept
    : str(zend_string_copy(other)) {}

String::String(std::string &other) noexcept
    : str(zend_string_init(other.c_str(), other.length(), false)) {}

String::String(const std::string &other) noexcept
    : str(zend_string_init(other.c_str(), other.length(), false)) {}

String::String(const zval &zv) noexcept {
  if (Z_TYPE(zv) == IS_STRING) {
    str = Z_STR(zv);
    zend_string_copy(str); // Increment the Reference
  }
}

String::~String() noexcept { Release(); }

inline uint64_t String::Hash() const noexcept {
  if (str == nullptr) {
    return 0;
  }

  if (ZSTR_H(str) == 0) {
    ZSTR_H(str) = zend_inline_hash_func(ZSTR_VAL(str), ZSTR_LEN(str));
  }

  return ZSTR_H(str);
}

void String::Release() noexcept {
  if (str && str != zend_empty_string) {
    zend_string_release(str);
  }
}

bool String::IsInterned() const {
  return str ? ZSTR_IS_INTERNED(str) : false;
}

uint32_t String::IncrementRef() noexcept {
  return zend_string_addref(str);
}

uint32_t String::DecrementRef() noexcept {
  return zend_string_delref(str);
}

uint32_t String::RefCount() const {
  return str ? zend_string_refcount(str) : 0;
}

bool String::IsEmpty() const noexcept {
  return str == nullptr || ZSTR_LEN(str) == 0;
}

bool String::IsNull() const noexcept { return str == nullptr; }

size_t String::Length() const noexcept { return ZSTR_LEN(str); }

const char *String::CString() const noexcept { return ZSTR_VAL(str); }

zend_string *String::ZendString() const noexcept { return str; }

zval String::IntoZval() const noexcept {
  zval zv;
  ZVAL_STR(&zv, str);
  return zv;
}

void String::IntoInterned() noexcept {
  if (str && str != zend_empty_string && !ZSTR_IS_INTERNED(str)) {
    str = zend_new_interned_string(str);
  }
}

String String::ToInterned() const noexcept {
  String newStr;

  if (str && str != zend_empty_string && !ZSTR_IS_INTERNED(str)) {
    newStr.str = zend_new_interned_string(str);
  }

  return newStr;
}

String::operator const zend_string *() const noexcept { return str; }

String::operator const char *() const noexcept { return ZSTR_VAL(str); }

String::operator zend_string *() const noexcept { return str; }

String::operator zval() const noexcept {
  zval zv;
  ZVAL_STR(&zv, str);
  return zv;
}

bool String::operator==(const String &rhs) const {
  return zend_string_equals(str, rhs.str);
}

bool String::operator==(String &rhs) const {
  return operator==(const_cast<const String &>(rhs));
}

bool String::operator!=(String &rhs) const {
  return !operator==(rhs);
}

bool String::operator!=(const String &rhs) const {
  return !operator==(rhs);
}

bool String::operator==(const char *rhs) const {
  return operator==(String(rhs));
}

bool String::operator!=(const char *rhs) const {
  return !operator==(rhs);
}

String &String::operator=(const String &&other) noexcept {
  str = std::move(other.str);
  return *this;
}
}