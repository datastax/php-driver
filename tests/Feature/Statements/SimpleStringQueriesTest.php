<?php

declare(strict_types=1);

namespace Cassandra\Tests\Feature\Statements;

use Cassandra;
use Cassandra\BatchStatement;

$keyspace = 'simple_string_queries';
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
      

      INSERT INTO $table
      (id, song_id, artist, title, album) VALUES
      (62c36092-82a1-3a00-93d1-46196ee77204, 756716f7-2e54-4715-9f00-91dcbea6cf50, 'Joséphine Baker', 'La Petite Tonkinoise', 'Bye Bye Blackbird');

      INSERT INTO $table
      (id, song_id, artist, title, album) VALUES
      (62c36092-82a1-3a00-93d1-46196ee77204, f6071e72-48ec-4fcb-bf3e-379c8a696488, 'Willi Ostermann', 'Die Mösch', 'In Gold');

      INSERT INTO $table
      (id, song_id, artist, title, album) VALUES
      (62c36092-82a1-3a00-93d1-46196ee77204, fbdf82ed-0063-4796-9c7c-a3d4f47b4b25, 'Mick Jager', 'Memo From Turner', 'Performance');
    CQL
    );
});

afterAll(function () use($keyspace) {
    dropKeyspace($keyspace);
});

test('A simple CQL string can be used to execute queries', function () use($keyspace, $table) {
    $session = scyllaDbConnection($keyspace);
    $result  = $session->execute("SELECT * FROM $table");

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

test('A simple CQL string can also be used to execute asynchronous queries', function () use($keyspace, $table) {
    $session = scyllaDbConnection($keyspace);
    $future  = $session->executeAsync("SELECT * FROM $table");
    $result = $future->get(1);
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

test('Simple CQL strings can also be used in batch statements',
    function () use ($keyspace, $table) {
        $session = scyllaDbConnection($keyspace);

        $batch = new BatchStatement(Cassandra::BATCH_UNLOGGED);

        $batch->add("INSERT INTO playlists
                   (id, song_id, artist, title, album) VALUES
                   (3a55adfc-bbf6-43bd-9428-e714f109b977, 82a954c4-750a-4ada-8e02-6b15c9bf3140 , 'The Beatles', 'Come Together', 'Abbey Road')");
        $batch->add("INSERT INTO playlists
                   (id, song_id, artist, title, album) VALUES
                   (3a55adfc-bbf6-43bd-9428-e714f109b977, 564e3c0d-bc3b-4d2d-8a34-679bb5247b71, 'Michael Jackson', 'Thriller', 'Thriller')");
        $batch->add("INSERT INTO playlists
                   (id, song_id, artist, title, album) VALUES
                   (3a55adfc-bbf6-43bd-9428-e714f109b977, 326ead7f-4c54-43f4-9b1b-40f7ca84cd5e, 'Pink Floyd', 'Another Brick in the Wall (Part I)', 'The Wall')");

        $session->execute($batch);

        $result = $session->execute("SELECT * FROM playlists");
        $expectations = [
            'Pink Floyd: Another Brick in the Wall (Part I) / The Wall',
            'The Beatles: Come Together / Abbey Road',
            'Michael Jackson: Thriller / Thriller',
            'Willi Ostermann: Die Mösch / In Gold',
            'Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird',
            'Mick Jager: Memo From Turner / Performance'
        ];

        foreach ($result as $key => $row) {
            $fullValue = sprintf("%s: %s / %s", $row['artist'], $row['title'], $row['album']);
            expect($fullValue)->toBe($expectations[$key]);
        }
    });
