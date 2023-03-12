add_library(cluster_builder STATIC BuilderHandlers.h)
add_library(ext_scylladb::cluster::builder ALIAS cluster_builder)
target_sources(
        cluster_builder
        PRIVATE
        Builder.c
        BuilderHandlers.c
        Cluster.c
        DefaultCluster.c
)

find_package(PHP REQUIRED)
set_target_properties(cluster_builder PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        cluster_builder
        PUBLIC
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libscylladb_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(cluster_builder PRIVATE -DDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(cluster_builder PRIVATE -DRELEASE)
endif ()

if (PHP_SCYLLADB_ENABLE_SANITIZERS)
    target_compile_options(
            cluster_builder PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer


    )
endif ()
