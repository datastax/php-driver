#pragma once

#include <php.h>


#if defined(__GNUC__)
#if __GNUC__ >= 3
#define PHP_DRIVER_ALWAYS_INLINE inline __attribute__((always_inline))
#define PHP_DRIVER_NEVER_INLINE __attribute__((noinline))
#else
#define PHP_DRIVER_ALWAYS_INLINE inline
#define PHP_DRIVER_NEVER_INLINE
#endif
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
