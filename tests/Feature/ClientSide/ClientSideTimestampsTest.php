<?php

namespace Cassandra\Tests\Feature\ClientSide;

use Cassandra;
use Cassandra\BatchStatement;
use Cassandra\Uuid;

/**
 * Cassandra uses timestamps to serialize write operations. That is, values with
 * more current timestamps are considered to be the most up-to-date version of
 * that information. By default, timestamps are assigned by Cassandra on the
 * server-side (`Cassandra\TimestampGenerator\ServerSide()`). The PHP Driver also
 * allows for timestamps to be assigned client-side in two ways:
 *
 * By providing a client-side timestamp generator such as `Cassandra\TimestampGenerator\Monotonic()`.
 *
 * Explicitly assigning a 'timestamp' when executing a statement or batch using execution options.
 */
$keyspace = 'client_side_timestamps';
$table = 'playlists';

$insertQuery = "INSERT INTO $table (id, song_id, artist, title, album) " .
    "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)";
$dataProvider = [
    'Joséphine Baker' => [
        'artist' => 'Joséphine Baker',
        'title' => 'La Petite Tonkinoise',
        'album' => 'Bye Bye Blackbird',
    ],
    'Willi Ostermann' => ['Willi Ostermann', 'Die Mösch', 'In Gold'],
    'Mick Jager' => ['Mick Jager', 'Memo From Turner', 'Performance'],
];

beforeAll(function () use ($keyspace, $table) {
    migrateKeyspace(<<<CQL
    CREATE KEYSPACE $keyspace WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
      };
      USE $keyspace;
      CREATE TABLE $table (
        id uuid,
        title text,
        album text,
        artist text,
        song_id uuid,
        PRIMARY KEY (id, title, album, artist)
      );
    CQL
    );
});

afterAll(function () use ($keyspace) {
    dropKeyspace($keyspace);
});

it('Create a simple statement with a client-side timestamp', function () use ($table, $keyspace, $insertQuery, $dataProvider) {
    $session = scyllaDbConnection($keyspace);

    $arguments = ['song_id' => new Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'), ...$dataProvider['Joséphine Baker']];
    $options = ['arguments' => $arguments, 'timestamp' => 1234];

    $session->execute($insertQuery, $options);

    $result = $session->execute("SELECT artist, title, album, WRITETIME(song_id) FROM $keyspace.$table")->first();

    expect($result['artist'])->toBe($dataProvider['Joséphine Baker']['artist'])
        ->and($result['title'])->toBe($dataProvider['Joséphine Baker']['title'])
        ->and($result['album'])->toBe($dataProvider['Joséphine Baker']['album'])
        ->and($result['writetime(song_id)']->value())->toBe('1234');
});

it('Create a new session using a timestamp generator', function () use ($table, $keyspace, $dataProvider, $insertQuery) {
    $session = scyllaDbConnection($keyspace);

    for ($i = 0; $i < 10; $i++) {
        $arguments = ['song_id' => new Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'), ...$dataProvider['Joséphine Baker']];
        $options = ['arguments' => $arguments, 'timestamp' => 1234];
        $session->execute($insertQuery, $options);
    }

    $result = $session->execute("SELECT artist, title, album, song_id FROM $keyspace.$table")->first();

    expect($result['artist'])->toBe($dataProvider['Joséphine Baker']['artist'])
        ->and($result['title'])->toBe($dataProvider['Joséphine Baker']['title'])
        ->and($result['album'])->toBe($dataProvider['Joséphine Baker']['album'])
        ->and($result['song_id']->uuid())->toBe('756716f7-2e54-4715-9f00-91dcbea6cf50');
});

it('Create a batch with a client-side timestamp', function () use ($table, $keyspace, $dataProvider, $insertQuery) {
    $session = scyllaDbConnection($keyspace);

    $prepared = $session->prepare($insertQuery);
    $batch = new BatchStatement(Cassandra::BATCH_LOGGED);

    $batch->add($prepared, [
        'song_id' => new Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
        'title' => $dataProvider['Joséphine Baker']['title'],
        'album' => $dataProvider['Joséphine Baker']['album'],
        'artist' => $dataProvider['Joséphine Baker']['artist'],
    ]);
    $batch->add($prepared, [new Uuid('f6071e72-48ec-4fcb-bf3e-379c8a696488'), ...$dataProvider['Willi Ostermann']]);
    $batch->add($prepared, [new Uuid('fbdf82ed-0063-4796-9c7c-a3d4f47b4b25'), ...$dataProvider['Mick Jager']]);

    $options = ['timestamp' => 5678];
    $session->execute($batch, $options);

    $result = $session->execute("SELECT artist, title, album, WRITETIME(song_id) FROM $keyspace.$table");

    foreach ($result as $row) {
        $expected = array_values($dataProvider[$row['artist']]);
        expect($row['artist'])->toBe($expected[0])
            ->and($row['title'])->toBe($expected[1])
            ->and($row['album'])->toBe($expected[2])
            ->and($row['writetime(song_id)']->value())->toBe('5678');
    }
});