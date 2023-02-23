#pragma once

namespace Zend::Internal::String
{
struct ByteIterator {
public:
  using value_type = uint8_t;
  using pointer    = uint8_t*;
  using reference  = uint8_t&;

private:
  pointer value = nullptr;
  size_t index  = 0;

public:
  explicit ByteIterator(pointer value, size_t index)
      : value(value)
      , index(index)
  {
  }

  reference operator*();

  pointer operator->();

  ByteIterator& operator++();

  ByteIterator operator++(int);

  friend bool
  operator==(const ByteIterator& a, const ByteIterator& b)
  {
    return a.value == b.value && a.index == b.index;
  }

  friend bool
  operator!=(const ByteIterator& a, const ByteIterator& b)
  {
    if (a.value != b.value) {
      return true;
    }

    // Pointers are the same
    return a.index != b.index;
  }
};
} // namespace Zend::Internal::String