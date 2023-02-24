add_library(zend_strings STATIC)
add_library(Zend::String ALIAS zend_strings)

find_package(PHP REQUIRED)

target_sources(zend_strings PRIVATE String.cpp ByteIterator.h ByteIterator.cpp)
set_target_properties(zend_strings PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        zend_strings PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PHP_INCLUDES}
)