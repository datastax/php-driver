<?php

use Cassandra\Map;
use Cassandra\Set;
use Cassandra\Inet;
use Cassandra\Type;
use Cassandra\Timestamp;
use Cassandra\Collection;
use Cassandra\SimpleStatement;

$keyspace = 'collections_testing';

beforeEach(function () use ($keyspace) {
    migrateKeyspace(<<<CQL
    CREATE KEYSPACE $keyspace WITH replication = {
        'class': 'SimpleStrategy',
        'replication_factor': 1
    };
    USE $keyspace;
    CREATE TABLE user (
        id int PRIMARY KEY,
        logins list<timestamp>,
        locations map<timestamp, double>,
        ip_addresses set<inet>
    );
    INSERT INTO user (id, logins, locations, ip_addresses)
    VALUES (
        0,
        ['2014-09-11 10:09:08+0000', '2014-09-12 10:09:00+0000'],
        {'2014-09-11 10:09:08+0000': 37.397357},
        {'200.199.198.197', '192.168.1.15'}
    );
    CQL);
});

afterEach(function () use ($keyspace) {
    dropKeyspace($keyspace);
});

test('Using Cassandra collections', function () use ($keyspace) {
    $session = scyllaDbConnection($keyspace);

    $statement = new SimpleStatement('SELECT * FROM user');

    $result = $session->execute($statement, []);
    $row = $result->first();

    /** @var \Cassandra\Collection */
    $logins = $row['logins'];
    expect($logins)
        ->toBeInstanceOf(Collection::class)
        ->toHaveCount(2);

    expect($logins->get(0))
        ->toBeInstanceOf(Timestamp::class)
        ->map(fn (Timestamp $value) => $value->time())
        ->toBe(DateTime::createFromFormat(DateTimeInterface::ISO8601, '2014-09-11T10:09:08+0000')->getTimestamp());
    expect($logins->get(1))
        ->toBeInstanceOf(Timestamp::class)
        ->map(fn (Timestamp $value) => $value->time())
        ->toBe(DateTime::createFromFormat(DateTimeInterface::ISO8601, '2014-09-12T10:09:00+0000')->getTimestamp());

    /** @var \Cassandra\Map */
    $locations = $row['locations'];
    expect($locations)
        ->toBeInstanceOf(Map::class)
        ->toHaveCount(1);

    expect($locations->keys())
        ->toHaveCount(1);

    expect($locations->keys()[0])
        ->toBeInstanceOf(Timestamp::class)
        ->map(fn (Timestamp $value) => $value->time())
        ->toBe(DateTime::createFromFormat(DateTimeInterface::ISO8601, '2014-09-11T10:09:08+0000')->getTimestamp());

    expect($locations->values()[0])
        ->toBe(37.397357);


    /** @var \Cassandra\Set */
    $ipAddresses = $row['ip_addresses'];
    expect($ipAddresses)
        ->toBeInstanceOf(Set::class)
        ->toHaveCount(2);

    expect($ipAddresses->values()[0])
        ->toBeInstanceOf(Inet::class)
        ->map(fn ($value) => (string)$value->address())
        ->toBe('192.168.1.15');

    expect($ipAddresses->values()[1])
        ->toBeInstanceOf(Inet::class)
        ->map(fn ($value) => (string)$value->address())
        ->toBe('200.199.198.197');
});
