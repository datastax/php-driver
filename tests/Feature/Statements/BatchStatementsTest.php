<?php

declare(strict_types=1);

namespace Cassandra\Tests\Feature\Statements;

use Cassandra;
use Cassandra\BatchStatement;
use Cassandra\Uuid;

/**
 * -> `Cassandra::BATCH_LOGGED` (Default) <-
 * This batch guarantees that either all or none of its statements will be executed.
 * This behavior is achieved by writing a batch log on the
 * coordinator, which slows down the execution somewhat
 * ----------------------------------------.
 * -> `Cassandra::BATCH_UNLOGGED` <-
 * this batch will not be verified when executed, which makes it
 * faster than a `LOGGED` batch, but means that some of its
 * statements might fail, while others - succeed.
 * ----------------------------------------.
 * -> `Cassandra::BATCH_COUNTER` <-
 * This batch is used for counter updates, which
 * are, unlike other writes, not idempotent.
 */

$keyspace = 'batch_statements';
$table = 'playlists';

beforeAll(function () use($keyspace, $table) {
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

test('Batch statements can contain simple and prepared statements', function () use ($keyspace, $table) {
    $session = scyllaDbConnection($keyspace);

    $insertQuery = "INSERT INTO playlists (id, song_id, artist, title, album) " .
        "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)";
    $prepared    = $session->prepare($insertQuery);
    $batch       = new BatchStatement(Cassandra::BATCH_LOGGED);

    $batch->add($prepared, [
        'song_id' => new Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
        'title'   => 'La Petite Tonkinoise',
        'album'   => 'Bye Bye Blackbird',
        'artist'  => 'Joséphine Baker'
    ]);

    $batch->add($insertQuery, array(
        new Uuid('f6071e72-48ec-4fcb-bf3e-379c8a696488'),
        'Willi Ostermann', 'Die Mösch', 'In Gold'
    ));

    $batch->add($prepared, array(
        new Uuid('fbdf82ed-0063-4796-9c7c-a3d4f47b4b25'),
        'Mick Jager', 'Memo From Turner', 'Performance'
    ));

    $session->execute($batch);

    $result = $session->execute("SELECT * FROM $keyspace.$table");
    $expectations = [
        'Willi Ostermann: Die Mösch / In Gold',
        'Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird',
        'Mick Jager: Memo From Turner / Performance'
    ];

    foreach ($result as $key => $row) {
        $fullValue = sprintf("%s: %s / %s", $row['artist'], $row['title'], $row['album']);
        expect($fullValue)->toBe($expectations[$key]);
    }
});