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

it('Accessing page info after loading next one', function () use ($keyspace, $dataProvider) {
    $session = scyllaDbConnection($keyspace);

    $options = ['page_size' => 10];
    $firstPageRows = $session->execute("SELECT * FROM paging_entries", $options);
    $secondPageRows = $firstPageRows->nextPage();

    expect($firstPageRows->isLastPage())->toBeFalse()
        ->and($secondPageRows->isLastPage())->toBeTrue()
        ->and($firstPageRows->count())->toBe(10)
        ->and($secondPageRows->count())->toBe(3);

});

it('Use paging state token to get next result', function () use ($keyspace, $dataProvider) {
    $session = scyllaDbConnection($keyspace);

    $query = 'SELECT * FROM paging_entries';
    $options = ['page_size' => 2];
    $result = $session->execute($query, $options);

    $resultedRows = [];
    while ($result->pagingStateToken()) {
        $options = array(
            'page_size' => 2,
            'paging_state_token' => $result->pagingStateToken()
        );

        $result = $session->execute($query, $options);

        foreach ($result as $row) {
            $resultedRows[$row['key']] = $row['value'];
        }
    }

    $expectedResult = [
        'm' => 12,
        'f' => 5,
        'g' => 6,
        'e' => 4,
        'd' => 3,
        'h' => 7,
        'l' => 11,
        'j' => 9,
        'i' => 8,
        'k' => 10,
        'b' => 1,
    ];

    expect($resultedRows)->toEqual($expectedResult);
});