add_library(type STATIC)
add_library(ext_scylladb::type ALIAS type)
target_sources(
        type
        PRIVATE
        Collection.cpp
        Custom.cpp
        Map.cpp
        Scalar.cpp
        Set.cpp
        Tuple.cpp
        UserType.cpp
)

find_package(PHP REQUIRED)
set_target_properties(type PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        type
        PUBLIC
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libscylladb_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(type PRIVATE -DDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(type PRIVATE -DRELEASE)
endif ()

if (PHP_SCYLLADB_ENABLE_SANITIZERS)
    target_compile_options(
            type PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer


    )
endif ()
