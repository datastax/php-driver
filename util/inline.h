#pragma once

#include <php.h>

#if DEBUG
#define PHP_DRIVER_ALWAYS_INLINE inline
#define PHP_DRIVER_NEVER_INLINE
#elif RELEASE
#if defined(__GNUC__)
#if __GNUC__ >= 3
#define PHP_DRIVER_ALWAYS_INLINE inline __attribute__((always_inline))
#define PHP_DRIVER_NEVER_INLINE __attribute__((noinline))
#else
#define PHP_DRIVER_ALWAYS_INLINE inline
#define PHP_DRIVER_NEVER_INLINE
#endif
#elif defined(_MSC_VER)
#define PHP_DRIVER_ALWAYS_INLINE __forceinline
#define PHP_DRIVER_NEVER_INLINE __declspec(noinline)
#else
#if __has_attribute(always_inline)
#define PHP_DRIVER_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define PHP_DRIVER_ALWAYS_INLINE inline
#endif
#if __has_attribute(noinline)
#define PHP_DRIVER_NEVER_INLINE __attribute__((noinline))
#else
#define PHP_DRIVER_NEVER_INLINE
#endif
#endif
#else
#define PHP_DRIVER_ALWAYS_INLINE
#define PHP_DRIVER_NEVER_INLINE
#endif
