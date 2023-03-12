add_library(exceptions STATIC)
add_library(ext_scylladb::exceptions ALIAS exceptions)
target_sources(
        exceptions
        PRIVATE
        AlreadyExistsException.cpp
        AuthenticationException.cpp
        ConfigurationException.cpp
        DivideByZeroException.cpp
        DomainException.cpp
        ExecutionException.cpp
        InvalidArgumentException.cpp
        InvalidQueryException.cpp
        InvalidSyntaxException.cpp
        IsBootstrappingException.cpp
        LogicException.cpp
        OverloadedException.cpp
        ProtocolException.cpp
        RangeException.cpp
        ReadTimeoutException.cpp
        RuntimeException.cpp
        ServerException.cpp
        TimeoutException.cpp
        TruncateException.cpp
        UnauthorizedException.cpp
        UnavailableException.cpp
        UnpreparedException.cpp
        ValidationException.cpp
        WriteTimeoutException.cpp
)

find_package(PHP REQUIRED)
set_target_properties(exceptions PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        exceptions
        PUBLIC
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libscylladb_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(exceptions PRIVATE -DDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(exceptions PRIVATE -DRELEASE)
endif ()

if (PHP_SCYLLADB_ENABLE_SANITIZERS)
    target_compile_options(
            exceptions PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer


    )
endif ()
