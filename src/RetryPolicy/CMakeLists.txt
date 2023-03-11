add_library(retry_policy STATIC)
add_library(ext_scylladb::retry_policy ALIAS retry_policy)
target_sources(
        retry_policy
        PRIVATE
        DefaultPolicy.c
        DowngradingConsistency.c
        Logging.c
        Fallthrough.c
)

find_package(PHP REQUIRED)
set_target_properties(retry_policy PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        retry_policy
        PUBLIC
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libscylladb_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(retry_policy PRIVATE -DDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(retry_policy PRIVATE -DRELEASE)
endif ()