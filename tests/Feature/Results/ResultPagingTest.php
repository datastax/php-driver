<?php

namespace Cassandra\Tests\Feature\Results;

/**
 * Starting with Cassandra native protocol v2 (used by Apache Cassandra 2.0),
 * paging through query results is allowed.
 *
 * Page size can be specified by setting the `$pageSize` attribute of
 * execution-options or cluster-wide, using `Cassandra\Cluster\Builder::withDefaultPageSize()`.
 *
 * Once a `Cassandra\Rows` object has been received, next page can be retrieved
 * using `Cassandra\Rows::nextPage()` or `Cassandra\Rows::nextPageAsync()`
 * methods for synchronous and asynchronous next page retrieval accordingly.
 *
 * To check if the next page is available, use `Cassandra\Rows::isLastPage()`
 */
$keyspace = 'result_paginator';
$table = 'paging_entries';
$dataProvider = [
    'a' => 0,
    'b' => 1,
    'c' => 2,
    'd' => 3,
    'e' => 4,
    'f' => 5,
    'g' => 6,
    'h' => 7,
    'i' => 8,
    'j' => 9,
    'k' => 10,
    'l' => 11,
    'm' => 12,
];


beforeAll(function () use ($keyspace, $table, $dataProvider) {
    $insertQuery = '';
    foreach ($dataProvider as $key => $value) {
        $insertQuery .= "INSERT INTO $table (key, value) VALUES ('$key', $value);" . PHP_EOL;
    }

    migrateKeyspace(<<<CQL
    CREATE KEYSPACE $keyspace WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE $keyspace;
      CREATE TABLE $table (key text, value int, PRIMARY KEY(key, value));
      $insertQuery
    CQL
    );
});

afterAll(function () use ($keyspace) {
    dropKeyspace($keyspace);
});

it('Paging through results synchronously', function () use ($keyspace, $dataProvider) {
    $session = scyllaDbConnection($keyspace);

    $options = ['page_size' => 5];
    $rows = $session->execute("SELECT * FROM paging_entries", $options);

    expect($rows->count())->toBe(5);

    while (true) {
        foreach ($rows as $row) {
            expect($row['value'])->toBe($dataProvider[$row['key']]);
        }

        if ($rows->isLastPage()) {
            break;
        }

        $rows = $rows->nextPage();
    }
});