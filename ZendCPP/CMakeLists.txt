add_library(zend STATIC Zend.cpp)

set_target_properties(zend PROPERTIES POSITION_INDEPENDENT_CODE ON)
find_package(PHP REQUIRED)

target_include_directories(zend PUBLIC ${PHP_INCLUDES})

add_subdirectory(String)
target_link_libraries(zend Zend::String)
