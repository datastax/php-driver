cmake_minimum_required(VERSION 3.18)

project(libphpcassandra LANGUAGES C)

find_package(Corrosion REQUIRED)

set(PHP_CASSANDRA_DRIVER_VERSION 1.3.7)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu17 -Wall")
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE Release)
set(PHP_INI_DIR /usr/local/lib/php)

set(CASSANDRA_LINK_LIBRARIES cassandra)

execute_process(COMMAND phpize WORKING_DIRECTORY ..)
execute_process(COMMAND ./configure WORKING_DIRECTORY ..)

file(READ ./config.h CASSANDRA_CONFIG_FILE)

add_definitions(-DHAVE_CONFIG_H)

execute_process(COMMAND php-config --includes OUTPUT_VARIABLE PHP_INCLUDES OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND php-config --extension-dir OUTPUT_VARIABLE PHP_EXTENSION_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PHP_INCLUDES}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PHP_INCLUDES}")

file(GLOB_RECURSE HEAD_FILES FOLLOW_SYMLINKS include/*.h)
file(GLOB_RECURSE SRC_LIST FOLLOW_SYMLINKS src/*.c)
file(GLOB_RECURSE UTILS_LIST FOLLOW_SYMLINKS util/*.c)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

link_directories(${LIBRARY_OUTPUT_PATH})
include_directories(BEFORE ./include ./)
# set_target_properties(lib-cassandra PROPERTIES OUTPUT_NAME "cassandra" VERSION ${PHP_CASSANDRA_DRIVER_VERSION})

set(php_cassandra_sources ${UTILS_LIST} ${SRC_LIST} php_driver.c)

add_library(ext-cassandra SHARED ${php_cassandra_sources})


corrosion_import_crate(
    MANIFEST_PATH ../utils/Cargo.toml
    PROFILE release
)

set_target_properties(ext-cassandra PROPERTIES PREFIX "")
set_target_properties(ext-cassandra PROPERTIES OUTPUT_NAME "cassandra")

target_link_libraries(ext-cassandra ${CASSANDRA_LINK_LIBRARIES})
target_link_libraries(ext-cassandra cassandra)

install(TARGETS ext-cassandra LIBRARY DESTINATION ${PHP_EXTENSION_DIR})
install(FILES ${HEAD_FILES} DESTINATION include/cassandra)
install(FILES cassandra.ini DESTINATION ${PHP_INI_DIR})
