<?php

declare(strict_types=1);

namespace Cassandra\Tests\Feature\Duration;

use Cassandra\Duration;
use Cassandra\Type;

$keyspace = 'duration_colletions';

beforeAll(function () use ($keyspace) {
    migrateKeyspace(<<<CQL
    CREATE KEYSPACE $keyspace WITH replication = {
            'class': 'SimpleStrategy',
            'replication_factor': 1
          };
          USE $keyspace;
          CREATE TYPE duration_user_type (duration1 duration, duration2 duration);
          CREATE TABLE durations (k text PRIMARY KEY,
                                  duration_list list<duration>,
                                  duration_tuple tuple<duration, duration>,
                                  duration_udt duration_user_type);
    CQL
    );
});

afterAll(function () use ($keyspace) {
    dropKeyspace($keyspace);
});

test('Use the duration type in collections', function () use($keyspace) {
    $session = scyllaDbConnection($keyspace);

    $list = Type::collection(Type::duration())->create(
        new Duration(1, 2, 3)
    );
    $list->add(new Duration(4, 5, 6));

    $tuple = Type::tuple(Type::duration(), Type::duration())->create(
        new Duration(1, 2, 3), new Duration(4, 5, 6)
    );

    $udt = Type::userType('duration1', Type::duration(), 'duration2', Type::duration())->create(
        'duration1', new Duration(1, 2, 3)
    );
    $udt->set('duration2', new Duration(4, 5, 6));


    # Insert collections containing durations into table
    $options = ['arguments' => ['key1', $list, $tuple, $udt]];
    $session->execute("INSERT INTO durations
          (k, duration_list, duration_tuple, duration_udt) VALUES
          (?, ?, ?, ?)", $options);

    # Select collections from table and print as WKT
    $rows = $session->execute("SELECT * FROM durations WHERE k = 'key1'");
    $row = $rows->first();

    $asWkt = array_map(function($k, $v) { return $k . ': ' . $v; },
        array_keys($row["duration_udt"]->values()), $row["duration_udt"]->values());

    expect(implode(', ', $row['duration_list']->values()))->toBe('1mo2d3ns, 4mo5d6ns')
        ->and(implode(', ', $row['duration_tuple']->values()))->toBe('1mo2d3ns, 4mo5d6ns')
        ->and(implode(', ', $asWkt))->toBe('duration1: 1mo2d3ns, duration2: 4mo5d6ns');
});
