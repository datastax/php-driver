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
beforeAll(function () use ($keyspace, $table) {
    migrateKeyspace(<<<CQL
    CREATE KEYSPACE $keyspace WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE $keyspace;
      CREATE TABLE $table (key text, value int, PRIMARY KEY(key, value));
      INSERT INTO $table (key, value) VALUES ('a', 0);
      INSERT INTO $table (key, value) VALUES ('b', 1);
      INSERT INTO $table (key, value) VALUES ('c', 2);
      INSERT INTO $table (key, value) VALUES ('d', 3);
      INSERT INTO $table (key, value) VALUES ('e', 4);
      INSERT INTO $table (key, value) VALUES ('f', 5);
      INSERT INTO $table (key, value) VALUES ('g', 6);
      INSERT INTO $table (key, value) VALUES ('h', 7);
      INSERT INTO $table (key, value) VALUES ('i', 8);
      INSERT INTO $table (key, value) VALUES ('j', 9);
      INSERT INTO $table (key, value) VALUES ('k', 10);
      INSERT INTO $table (key, value) VALUES ('l', 11);
      INSERT INTO $table (key, value) VALUES ('m', 12);
    CQL
);
});

afterAll(function () use ($keyspace) {
    dropKeyspace($keyspace);
});

it('Paging through results synchronously', function () use ($keyspace) {
    $session = scyllaDbConnection($keyspace);

    $options   = ['page_size' => 5];
    $rows      = $session->execute("SELECT * FROM paging_entries", $options);

    while (true) {
        echo "entries in page: " . $rows->count() . "\n";

        foreach ($rows as $row) {
            echo "key: " . $row['key'] . ", value: " . $row['value'] . "\n";
        }

        if ($rows->isLastPage()) {
            break;
        }

        $rows = $rows->nextPage();
    }
});