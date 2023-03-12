#include "ByteIterator.h"

#include <cstdint>

using namespace Zend::Internal::String;

ByteIterator::reference ByteIterator::operator*() {
  return (reference)value[index];
}

ByteIterator::pointer ByteIterator::operator->() {
  return (pointer) & value[index];
}

ByteIterator &ByteIterator::operator++() {
  index++;
  return *this;
}

ByteIterator ByteIterator::operator++(int) {
  auto tmp = *this;

  ++(*this);
  return tmp;
}