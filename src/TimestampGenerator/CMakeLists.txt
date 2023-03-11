add_library(timestamp_generator STATIC)
add_library(ext_scylladb::timestamp_generator ALIAS timestamp_generator)
target_sources(
        timestamp_generator
        PRIVATE
        Monotonic.c
        ServerSide.c
)

find_package(PHP REQUIRED)
set_target_properties(timestamp_generator PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        timestamp_generator
        PUBLIC
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libscylladb_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(timestamp_generator PRIVATE -DDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(timestamp_generator PRIVATE -DRELEASE)
endif ()