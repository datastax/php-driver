add_library(src STATIC)
add_library(ext_scylladb::src ALIAS src)
target_sources(
        src
        PRIVATE
        Aggregate.c
        BatchStatement.c
        Bigint.c
        Blob.c
        Cluster.c
        Collection.c
        Column.c
        Core.c
        Custom.c
        Date.c
        Decimal.c
        DefaultAggregate.c
        DefaultCluster.c
        DefaultColumn.c
        DefaultFunction.c
        DefaultIndex.c
        DefaultKeyspace.c
        DefaultMaterializedView.c
        DefaultSchema.c
        DefaultSession.c
        DefaultTable.c
        Duration.c
        Exception.c
        ExecutionOptions.c
        Float.c
        Function.c
        Future.c
        FuturePreparedStatement.c
        FutureRows.c
        FutureSession.c
        FutureClose.c
        FutureValue.c
        Index.c
        Inet.c
        Keyspace.c
        Map.c
        MaterializedView.c
        Numeric.c
        PreparedStatement.c
        RetryPolicy.c
        Rows.c
        Schema.c
        Session.c
        Set.c
        SimpleStatement.c
        Smallint.c
        SSLOptions.c
        Statement.c
        Table.c
        Time.c
        Timestamp.c
        TimestampGenerator.c
        Timeuuid.c
        Tinyint.c
        Tuple.c
        Type.c
        UserTypeValue.c
        Uuid.c
        UuidInterface.c
        Value.c
        Varint.c

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