cmake_minimum_required(VERSION 3.22.1)
project(php-ext-scylladb LANGUAGES C CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/CPM.cmake)
include(cmake/TargetOptimizations.cmake)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

option(PHP_SCYLLADB_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(PHP_SCYLLADB_ENABLE_COVERAGE "Enable coverage" OFF)
option(PHP_SCYLLADB_OPTIMISE_FOR_CURRENT_MACHINE "Optimise for current machine" OFF)

option(PHP_SCYLLADB_LIBUV_STATIC "Statically link libuv" OFF)
option(PHP_SCYLLADB_LIBSCYLLADB_STATIC "Statically link libscylladb" OFF)

find_package(PHP REQUIRED)
find_package(PkgConfig REQUIRED)


file(GLOB_RECURSE SRC_LIST FOLLOW_SYMLINKS src/*.c)
file(GLOB_RECURSE UTILS_LIST FOLLOW_SYMLINKS util/*.c)
set(php_scylladb_sources ${UTILS_LIST} ${SRC_LIST} php_driver.c)

# use find package to find libuv and libscylla

add_library(ext_scylladb SHARED ${php_scylladb_sources})
add_compile_options(
        -Wall -Wextra -Wno-long-long -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-result -Wno-variadic-macros -Wno-extra-semi -pthread
        $<$<CONFIG:DEBUG>:-ggdb3
)

if (PHP_SCYLLADB_LIBSCYLLADB_STATIC)
    target_compile_definitions(ext_scylladb PRIVATE -DSCYLLADB_STATIC)
    pkg_check_modules(SCYLLADB REQUIRED cassandra_static)
    message(STATUS "Using static libscylladb")
else ()
    pkg_check_modules(SCYLLADB REQUIRED cassandra)
    message(STATUS "Using dynamic libscylladb")
endif ()

if (SCYLLADB_FOUND)
    target_link_libraries(ext_scylladb PRIVATE ${SCYLLADB_LIBRARIES})
    target_include_directories(ext_scylladb PRIVATE ${SCYLLADB_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "libcassandra not found")
endif ()


if (PHP_SCYLLADB_LIBUV_STATIC)
    target_compile_definitions(ext_scylladb PRIVATE -DUV_STATIC)
    pkg_check_modules(LIBUV REQUIRED libuv_static)
    message(STATUS "Using static libuv")
else ()
    pkg_check_modules(LIBUV REQUIRED libuv)
    message(STATUS "Using dynamic libuv")
endif ()


if (LIBUV_FOUND)
    target_link_libraries(ext_scylladb PRIVATE ${LIBUV_LIBRARIES})
    target_include_directories(ext_scylladb PRIVATE ${LIBUV_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "libuv not found")
endif ()


target_include_directories(ext_scylladb
        PRIVATE "${PHP_INCLUDES}"
        PUBLIC "${PROJECT_SOURCE_DIR}/include"
        PUBLIC "${PROJECT_SOURCE_DIR}"
        )
target_compile_definitions(ext_scylladb PRIVATE -DCOMPILE_DL_CASSANDRA)
set_target_properties(ext_scylladb PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(ext_scylladb PROPERTIES PREFIX "")
set_target_properties(ext_scylladb PROPERTIES OUTPUT_NAME "cassandra")

php_scylladb_optimize(ext_scylladb)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cassandra.so" DESTINATION "${PHP_EXTENSION_DIR}")