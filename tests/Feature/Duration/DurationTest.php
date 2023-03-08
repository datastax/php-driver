<?php
declare(strict_types=1);

namespace Cassandra\Tests\Feature\Duration;

use Cassandra\Duration;

$keyspace = 'duration';
$table = 'durations';

beforeAll(function () use ($keyspace, $table) {
    migrateKeyspace(<<<CQL
    CREATE KEYSPACE $keyspace WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE $keyspace;
      CREATE TABLE $table (alias text PRIMARY KEY, duration_at duration);
    CQL
    );
});

afterAll(function () use ($keyspace) {
    dropKeyspace($keyspace);
});

test('Use the duration type', function () use($keyspace, $table) {
    $session = scyllaDbConnection($keyspace);

    $durations = [
        ['two_days', new Duration(2, 0, 0)],
        ['twelve_hours', new Duration(0, 12, 0)],
        ['three_seconds', new Duration(0, 0, 3 * (1000 ** 3))],
        ['two_days_twelve_hours_and_three_seconds', new Duration(2, 12, 3 * (1000 ** 3))]
    ];

    foreach ($durations as $duration) {
        $options = ['arguments' => $duration];
        $session->execute("INSERT INTO $table (alias, duration_at) VALUES (?, ?)", $options);
    }

    $rows = $session->execute("SELECT * FROM $table");

    $expectations = [
        'twelve_hours: 0mo12d0ns',
        'three_seconds: 0mo0d3000000000ns',
        'two_days_twelve_hours_and_three_seconds: 2mo12d3000000000ns',
        'two_days: 2mo0d0ns'
    ];

    foreach ($rows as $key => $row) {
        $fullValue = sprintf("%s: %s", $row['alias'], $row['duration_at']);
        expect($fullValue)->toBe($expectations[$key]);
    }
});
