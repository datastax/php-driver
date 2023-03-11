#pragma once

#include <php.h>

#if defined(__GNUC__)
#if __GNUC__ >= 3
#define ZENDCPP_ALWAYS_INLINE inline __attribute__((always_inline))
#define ZENDCPP_NEVER_INLINE __attribute__((noinline))
#else
#define ZENDCPP_ALWAYS_INLINE inline
#define ZENDCPP_NEVER_INLINE
#endif
#else
#if __has_attribute(always_inline)
#define ZENDCPP_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ZENDCPP_ALWAYS_INLINE inline
#endif
#if __has_attribute(noinline)
#define ZENDCPP_NEVER_INLINE __attribute__((noinline))
#else
#define ZENDCPP_NEVER_INLINE
#endif
#endif

#ifndef ZEND_OBJECT_OFFSET_MEMBER
#define ZEND_OBJECT_OFFSET_MEMBER zval
#endif

namespace ZendCPP
{
template <typename T> ZENDCPP_ALWAYS_INLINE T *ObjectFetch(zend_object *obj)
{
    auto offset = reinterpret_cast<std::size_t>(&reinterpret_cast<T *>(0)->ZEND_OBJECT_OFFSET_MEMBER);
    auto *casted = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(obj) - offset);
    return casted;
}
} // namespace ZendCPP