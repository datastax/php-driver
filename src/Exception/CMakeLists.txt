add_library(exceptions STATIC)
add_library(ext_scylladb::exceptions ALIAS exceptions)
target_sources(
        exceptions
        PRIVATE
        AlreadyExistsException.c
        AuthenticationException.c
        ConfigurationException.c
        DivideByZeroException.c
        DomainException.c
        ExecutionException.c
        InvalidArgumentException.c
        InvalidQueryException.c
        InvalidSyntaxException.c
        IsBootstrappingException.c
        LogicException.c
        OverloadedException.c
        ProtocolException.c
        RangeException.c
        ReadTimeoutException.c
        RuntimeException.c
        ServerException.c
        TimeoutException.c
        TruncateException.c
        UnauthorizedException.c
        UnavailableException.c
        UnpreparedException.c
        ValidationException.c
        WriteTimeoutException.c
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