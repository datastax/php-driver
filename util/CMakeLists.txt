add_library(util STATIC)
add_library(ext_scylladb::utils ALIAS util)
target_sources(
        util
        PRIVATE
        src/bytes.c
        src/collections.c
        src/consistency.c
        src/future.c
        src/hash.c
        src/inet.c
        src/ref.c
        src/result.c
        src/types.c
        src/uuid_gen.c
)

find_package(PHP REQUIRED)

target_include_directories(
        util
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR} # TODO: Should be removed
        ${PROJECT_BINARY_DIR}
)