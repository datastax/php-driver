add_library(src STATIC)
add_library(ext_scylladb::src ALIAS src)
target_sources(
        src
        PRIVATE
        Aggregate.cpp
        BatchStatement.cpp
        Bigint.cpp
        Blob.cpp
        Collection.cpp
        Column.cpp
        Core.cpp
        Custom.cpp
        Date.cpp
        Decimal.cpp
        DefaultAggregate.cpp
        DefaultColumn.cpp
        DefaultFunction.cpp
        DefaultIndex.cpp
        DefaultKeyspace.cpp
        DefaultMaterializedView.cpp
        DefaultSchema.cpp
        DefaultSession.cpp
        DefaultTable.cpp
        Duration.cpp
        Exception.cpp
        ExecutionOptions.cpp
        Float.cpp
        Function.cpp
        Future.cpp
        FuturePreparedStatement.cpp
        FutureRows.cpp
        FutureSession.cpp
        FutureClose.cpp
        FutureValue.cpp
        Index.cpp
        Inet.cpp
        Keyspace.cpp
        Map.cpp
        MaterializedView.cpp
        Numeric.cpp
        PreparedStatement.cpp
        RetryPolicy.cpp
        Rows.cpp
        Schema.cpp
        Session.cpp
        Set.cpp
        SimpleStatement.cpp
        Smallint.cpp
        SSLOptions.cpp
        Statement.cpp
        Table.cpp
        Time.cpp
        Timestamp.cpp
        TimestampGenerator.cpp
        Timeuuid.cpp
        Tinyint.cpp
        Tuple.cpp
        Type.cpp
        UserTypeValue.cpp
        Uuid.cpp
        UuidInterface.cpp
        Value.cpp
        Varint.cpp

)

find_package(PHP REQUIRED)
set_target_properties(src PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(
        src
        PUBLIC
        ${PHP_INCLUDES}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
        ${libscylladb_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(src PRIVATE -DDEBUG)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_definitions(src PRIVATE -DRELEASE)
endif ()

if (PHP_SCYLLADB_ENABLE_SANITIZERS)
    target_compile_options(
            src PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer


    )
endif ()
