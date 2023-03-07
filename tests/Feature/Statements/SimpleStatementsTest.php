<?php
declare(strict_types=1);

use Cassandra\SimpleStatement;
use Cassandra\Uuid;

$keyspace = 'simple_statements';
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

afterAll(function () use($keyspace) {
    dropKeyspace($keyspace);
});

test('Simple statements are initialized with a CQL string', function () use ($keyspace, $table) {
    $session = scyllaDbConnection($keyspace);
    $statement = new SimpleStatement("SELECT * FROM $table");
    $result    = $session->execute($statement);
    expect($result->count())->toBe(0);
});

test('Simple statements support positional arguments', function () use($keyspace, $table) {
    $session = scyllaDbConnection($keyspace);

    $statement = new SimpleStatement(
        "INSERT INTO $table (id, song_id, artist, title, album) " .
        "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)"
    );

    $songs = [
        [
            new Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
            'Joséphine Baker',
            'La Petite Tonkinoise',
            'Bye Bye Blackbird'
        ],
        [
            new Uuid('f6071e72-48ec-4fcb-bf3e-379c8a696488'),
            'Willi Ostermann',
            'Die Mösch',
            'In Gold'
        ],
        [
            new Uuid('fbdf82ed-0063-4796-9c7c-a3d4f47b4b25'),
            'Mick Jager',
            'Memo From Turner',
            'Performance'
        ],
    ];

    foreach ($songs as $song) {
        $options = array('arguments' => $song);
        $session->execute($statement, $options);
    }

    $rows = $session->execute("SELECT * FROM $keyspace.$table");

    $expectations = [
        'Willi Ostermann: Die Mösch / In Gold',
        'Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird',
        'Mick Jager: Memo From Turner / Performance'
    ];

    foreach ($rows as $key => $row) {
        $fullValue = sprintf("%s: %s / %s", $row['artist'], $row['title'], $row['album']);
        expect($fullValue)->toBe($expectations[$key]);
    }
});

test('Simple statements also support named arguments', function() use ($keyspace, $table) {
    $session = scyllaDbConnection($keyspace);

    $statement = new SimpleStatement(
        "INSERT INTO $table (id, song_id, artist, title, album) " .
        "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)"
    );

    $songs = [
        [
            'song_id' => new Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
            'artist'  => 'Joséphine Baker',
            'title'   => 'La Petite Tonkinoise',
            'album'   => 'Bye Bye Blackbird'
        ],
        [
            'song_id' => new Uuid('f6071e72-48ec-4fcb-bf3e-379c8a696488'),
            'artist'  => 'Willi Ostermann',
            'title'   => 'Die Mösch',
            'album'   => 'In Gold'
        ],
        [
            'song_id' => new Uuid('fbdf82ed-0063-4796-9c7c-a3d4f47b4b25'),
            'artist'  => 'Mick Jager',
            'title'   => 'Memo From Turner',
            'album'   => 'Performance'
        ],
    ];

    foreach ($songs as $song) {
        $session->execute($statement, array('arguments' => $song));
    }

    $rows = $session->execute("SELECT * FROM $keyspace.$table");

    $expectations = [
        'Willi Ostermann: Die Mösch / In Gold',
        'Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird',
        'Mick Jager: Memo From Turner / Performance'
    ];

    foreach ($rows as $key => $row) {
        $fullValue = sprintf("%s: %s / %s", $row['artist'], $row['title'], $row['album']);
        expect($fullValue)->toBe($expectations[$key]);
    }
});

test('Simple statements also supports ":name" arguments', function () use ($keyspace, $table) {
    $session = scyllaDbConnection($keyspace);

    $songs = [
        [
            'song_id' => new Uuid('756716f7-2e54-4715-9f00-91dcbea6cf50'),
            'artist'  => 'Joséphine Baker',
            'title'   => 'La Petite Tonkinoise',
            'album'   => 'Bye Bye Blackbird'
        ],
    ];

    foreach ($songs as $song) {
        var_dump($songs);
        $options = ['arguments' => $song];
        $session->execute(
            "INSERT INTO $table (id, song_id, artist, title, album) " .
            "VALUES (62c36092-82a1-3a00-93d1-46196ee77204, ?, ?, ?, ?)",
            $options
        );
    }

    $statement = new SimpleStatement(
        "SELECT * FROM $keyspace.$table " .
        "WHERE id = :id AND artist = :artist AND title = :title AND album = :album"
    );

    $options = ['arguments' =>
        [
            'id'     => new Cassandra\Uuid('62c36092-82a1-3a00-93d1-46196ee77204'),
            'artist' => 'Joséphine Baker',
            'title'  => 'La Petite Tonkinoise',
            'album'  => 'Bye Bye Blackbird'
        ]
    ];

    $result = $session->execute($statement, $options);
    $row = $result->first();

    $fullValue = sprintf("%s: %s / %s", $row['artist'], $row['title'], $row['album']);
    expect($fullValue)->toBe('Joséphine Baker: La Petite Tonkinoise / Bye Bye Blackbird');
});