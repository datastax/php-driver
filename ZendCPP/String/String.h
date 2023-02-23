#pragma once

#include <php.h>

#include "ByteIterator.h"

namespace Zend
{
using string =
  // TODO: Implement Forward iterator
  class String
{
private:
  zend_string* str;

  void Release() noexcept;

public:
  using iterator = Internal::String::ByteIterator;

  String() noexcept
      : str(zend_empty_string)
  {
  }

  String(const char* other, size_t len) noexcept;

  explicit String(const char* other) noexcept;

  explicit String(const zend_string* other) noexcept;

  explicit String(zend_string* other) noexcept;

  explicit String(std::string& other) noexcept;

  explicit String(const std::string& other) noexcept;

  explicit String(const zval& zv) noexcept;

  String(const String& other) noexcept
      : str(zend_string_copy(other.str))
  {
  }

  String(const String&& other) noexcept
      : str(std::move(other.str))
  {
  }

  String(zend_string*&& other) noexcept
      : str(std::move(other))
  {
  }

  [[maybe_unused]] [[nodiscard]] uint64_t Hash() const noexcept;

  [[maybe_unused]] [[nodiscard]] bool IsInterned() const;

  [[maybe_unused]] uint32_t IncrementRef() noexcept;

  [[maybe_unused]] uint32_t DecrementRef() noexcept;

  [[maybe_unused]] [[nodiscard]] uint32_t RefCount() const;

  [[maybe_unused]] [[nodiscard]] bool IsEmpty() const noexcept;

  [[maybe_unused]] [[nodiscard]] bool IsNull() const noexcept;

  [[maybe_unused]] [[nodiscard]] size_t Length() const noexcept;

  [[maybe_unused]] [[nodiscard]] const char* CString() const noexcept;

  [[maybe_unused]] [[nodiscard]] zend_string* ZendString() const noexcept;

  [[maybe_unused]] [[nodiscard]] zval IntoZval() const noexcept;

  [[maybe_unused]] void IntoInterned() noexcept;

  [[maybe_unused]] [[nodiscard]] String ToInterned() const noexcept;

  operator const zend_string*() const noexcept;

  operator const char*() const noexcept;

  operator zend_string*() const noexcept;

  operator zval() const noexcept;

  String& operator=(const String&& other) noexcept;

  bool operator==(const String& rhs) const;
  bool operator!=(const char* rhs) const;
  bool operator==(const char* rhs) const;

  bool operator==(String& rhs) const;

  bool operator!=(String& rhs) const;

  bool operator!=(const String& rhs) const;

  Zend::Internal::String::ByteIterator begin() noexcept;

  Zend::Internal::String::ByteIterator end() noexcept;

  ~String() noexcept;
};
} // namespace Zend
