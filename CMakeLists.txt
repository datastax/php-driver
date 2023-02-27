cmake_minimum_required(VERSION 3.24)
project(php-ext-scylladb LANGUAGES C CXX)

# if build type is not set, default to debug
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

include(GNUInstallDirs)
include(CheckSymbolExists)
include(CheckIncludeFile)
include(CMakePackageConfigHelpers)
include(cmake/CPM.cmake)
include(cmake/TargetOptimizations.cmake)

set(PHP_SCYLLADB_VERSION_MAJOR 1)
set(PHP_SCYLLADB_VERSION_MINOR 3)
set(PHP_SCYLLADB_VERSION_PATCH 9)
set(PHP_SCYLLADB_STABILITY "devel")
set(PHP_SCYLLADB_VERSION_FULL "${PHP_SCYLLADB_VERSION_MAJOR}.${PHP_SCYLLADB_VERSION_MINOR}.${PHP_SCYLLADB_VERSION_PATCH}-${PHP_SCYLLADB_STABILITY}")
set(PHP_SCYLLADB_VERSION "${PHP_SCYLLADB_VERSION_MAJOR}.${PHP_SCYLLADB_VERSION_MINOR}.${PHP_SCYLLADB_VERSION_PATCH}-dev")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

option(PHP_SCYLLADB_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(PHP_SCYLLADB_ENABLE_COVERAGE "Enable coverage" OFF)
option(PHP_SCYLLADB_OPTIMISE_FOR_CURRENT_MACHINE "Optimise for current machine" OFF)

option(PHP_SCYLLADB_LIBUV_STATIC "Statically link libuv" ON)
option(PHP_SCYLLADB_LIBSCYLLADB_STATIC "Statically link libscylladb" ON)

find_package(PHP REQUIRED)

if (PHP_SCYLLADB_LIBUV_STATIC)
    set(LIBUV_BUILD_SHARED OFF)
else ()
    set(LIBUV_BUILD_SHARED ON)
endif ()

CPMAddPackage(
        NAME libuv
        VERSION 1.44.2
        URL https://github.com/libuv/libuv/tarball/v1.44.2
        URL_HASH MD5=3e22e24d53aab67252907dfa004a6b53
        OPTIONS
        "BUILD_TESTING OFF"
        "BUILD_BENCHMARKS OFF"
        "LIBUV_BUILD_SHARED ${LIBUV_BUILD_SHARED}"
        "CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -fPIC"
        "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC"
        
)

set(LIBUV_ROOT_DIR ${libuv_BINARY_DIR})
if (PHP_SCYLLADB_LIBUV_STATIC)
    set(LIBUV_LIBRARY "${libuv_BINARY_DIR}/libuv_a.a")
else ()
    set(LIBUV_LIBRARY "${libuv_BINARY_DIR}/libuv.so")
endif ()


if (PHP_SCYLLADB_LIBSCYLLADB_STATIC)
    set(CASS_BUILD_STATIC ON)
    set(CASS_BUILD_SHARED OFF)
else ()
    set(CASS_BUILD_STATIC OFF)
    set(CASS_BUILD_SHARED ON)
endif ()


CPMAddPackage(
        NAME libscylladb
        VERSION 2.16.2
        URL https://github.com/scylladb/cpp-driver/tarball/2.16.2b
        URL_HASH MD5=67e6c74cdd6c9692b2745bfc28dccdaa
        OPTIONS
        "CASS_CPP_STANDARD 17"
        "CASS_BUILD_STATIC ${CASS_BUILD_STATIC}"
        "CASS_BUILD_SHARED ${CASS_BUILD_SHARED}"
        "CASS_USE_STD_ATOMIC ON"
        "CASS_USE_TIMERFD ON"
        "CASS_USE_LIBSSH2 ON"
        "CASS_USE_ZLIB ON"
        "CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -fPIC"
        "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC"
        "LIBUV_LIBRARY ${LIBUV_LIBRARY}"
        "LIBUV_INCLUDE_DIR ${libuv_SOURCE_DIR}/include"
)



file(GLOB_RECURSE SRC_LIST FOLLOW_SYMLINKS src/*.c)
file(GLOB_RECURSE HEADERS_LIST FOLLOW_SYMLINKS include/*.h)
set(php_scylladb_sources ${SRC_LIST} php_driver.c)

add_subdirectory(ZendCPP)
add_subdirectory(util)

add_library(ext_scylladb SHARED)
target_optimize(ext_scylladb ${PHP_SCYLLADB_OPTIMISE_FOR_CURRENT_MACHINE})
add_compile_options(
        -fPIC -Wall -Wextra -Wno-long-long -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-result -Wno-variadic-macros -Wno-extra-semi -pthread
        $<$<CONFIG:DEBUG>:-ggdb3
)
target_include_directories(
        ext_scylladb PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libuv_SOURCE_DIR}/include
        ${libscylladb_SOURCE_DIR}/include
        ${PHP_INCLUDES}
)
target_sources(ext_scylladb PUBLIC FILE_SET headers TYPE HEADERS FILES "${HEADERS_LIST}" "${PROJECT_BINARY_DIR}/config.h")
target_sources(ext_scylladb PRIVATE ${php_scylladb_sources})

target_link_libraries(
        ext_scylladb PRIVATE
        zend
        ext_scylladb::utils
)

if (PHP_SCYLLADB_LIBUV_STATIC)
    target_compile_definitions(ext_scylladb PRIVATE -DUV_STATIC)
    target_link_libraries(ext_scylladb PRIVATE uv_a)
else ()
    target_link_libraries(ext_scylladb PRIVATE uv)
endif ()

if (PHP_SCYLLADB_LIBSCYLLADB_STATIC)
    target_compile_definitions(ext_scylladb PRIVATE -DSCYLLADB_STATIC)
    target_link_libraries(ext_scylladb PRIVATE cassandra_static)
else ()
    target_link_libraries(ext_scylladb PRIVATE "libscylla-cpp-driver.so")
endif ()

check_include_file("dlfcn.h" HAVE_DLFCN_H)
check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("unistd.h" HAVE_UNISTD_H)

set(HAVE_STRINGS_H 0)


configure_file(
        config.in
        config.h
        USE_SOURCE_PERMISSIONS
        NEWLINE_STYLE UNIX
)

configure_file(
        version.h.in
        version.h
        USE_SOURCE_PERMISSIONS
        NEWLINE_STYLE UNIX
)

target_compile_definitions(ext_scylladb PRIVATE -DCOMPILE_DL_CASSANDRA -DHAVE_CONFIG_H)
set_target_properties(ext_scylladb PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(ext_scylladb PROPERTIES PREFIX "")
set_target_properties(ext_scylladb PROPERTIES OUTPUT_NAME "cassandra")

install(TARGETS ext_scylladb DESTINATION "${PHP_EXTENSION_DIR}")
install(TARGETS ext_scylladb FILE_SET headers DESTINATION "${PHP_INCLUDES_DIR}/ext/cassandra")
