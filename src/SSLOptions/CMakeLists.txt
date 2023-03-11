add_library(ssl_options STATIC)
add_library(ext_scylladb::ssl_options ALIAS ssl_options)
target_sources(
        ssl_options
        PRIVATE
        Builder.c
)

find_package(PHP REQUIRED)
set_target_properties(ssl_options PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        ssl_options
        PUBLIC
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libscylladb_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(ssl_options PRIVATE -DDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(ssl_options PRIVATE -DRELEASE)
endif ()