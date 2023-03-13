add_library(util STATIC)
add_library(ext_scylladb::utils ALIAS util)
target_sources(
        util
        PRIVATE
        src/bytes.cpp
        src/collections.cpp
        src/future.cpp
        src/hash.cpp
        src/inet.cpp
        src/ref.cpp
        src/result.cpp
        src/types.cpp
        src/uuid_gen.cpp
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

if (PHP_SCYLLADB_ENABLE_SANITIZERS)
    target_compile_options(
            util PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer


    )
endif ()
