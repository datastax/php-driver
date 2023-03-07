add_library(util STATIC)
add_library(ext_scylladb::utils ALIAS util)
target_sources(
        util
        PRIVATE
        src/bytes.c
        src/collections.c
        src/future.c
        src/hash.c
        src/inet.c
        src/ref.c
        src/result.c
        src/types.c
        src/uuid_gen.c
)

find_package(PHP REQUIRED)
set_target_properties(util PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        util
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libscylladb_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(util PRIVATE -DDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(util PRIVATE -DRELEASE)
endif ()