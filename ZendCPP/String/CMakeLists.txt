add_library(zend_strings STATIC)
add_library(Zend::String ALIAS zend_strings)

find_package(PHP REQUIRED)

target_sources(zend_strings PRIVATE String.cpp ByteIterator.h ByteIterator.cpp)
target_include_directories(
        zend_strings PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PHP_INCLUDES}
)