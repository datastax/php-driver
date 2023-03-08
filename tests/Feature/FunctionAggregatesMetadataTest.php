<?php

declare(strict_types=1);

$keyspace = 'functions_aggregates_metadata';

//beforeAll(function () use ($keyspace) {
//    migrateKeyspace(<<<CQL
//    CREATE KEYSPACE $keyspace WITH replication = {
//        'class': 'SimpleStrategy',
//        'replication_factor': 1
//      } AND DURABLE_WRITES = false;
//      USE $keyspace;
//      CREATE OR REPLACE FUNCTION fLog (input double) CALLED ON NULL INPUT RETURNS double LANGUAGE java AS 'return Double.valueOf(Math.log(input.doubleValue()));';
//      CREATE OR REPLACE FUNCTION avgState ( state tuple<int,bigint>, val int ) CALLED ON NULL INPUT RETURNS tuple<int,bigint> LANGUAGE java AS 'if (val !=null) { state.setInt(0, state.getInt(0)+1); state.setLong(1, state.getLong(1)+val.intValue()); } return state;';
//      CREATE OR REPLACE FUNCTION avgFinal ( state tuple<int,bigint> ) CALLED ON NULL INPUT RETURNS double LANGUAGE java AS 'double r = 0; if (state.getInt(0) == 0) return null; r = state.getLong(1); r/= state.getInt(0); return Double.valueOf(r);';
//      CREATE AGGREGATE IF NOT EXISTS average ( int ) SFUNC avgState STYPE tuple<int,bigint> FINALFUNC avgFinal INITCOND (0,0);
//    CQL);
//});
//
//afterAll(function () use($keyspace) {
//    dropKeyspace($keyspace);
//});
//
//
//it('Getting a functions metadata', function () use($keyspace) {
//    $session = scyllaDbConnection($keyspace);
//    $schema = $session->schema();
//
//    $function = $schema->keyspace($keyspace)->function("flog", Cassandra\Type::double());
//
//    expect($function->simpleName())->toBe('flog')
//        ->and($function->signature())->toBe('flog(double)')
//        ->and($function->language())->toBe('java')
//        ->and($function->body())->toBe('return Double.valueOf(Math.log(input.doubleValue()));');
//
////    echo "Arguments: " . var_export($function->arguments(), true) . "\n";
////    echo "ReturnType: " . var_export($function->returnType(), true) . "\n";
////    echo "IsCalledOnNullInput: " . ($function->isCalledOnNullInput() ? "true" : "false") . "\n";
//});