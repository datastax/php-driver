<?php

declare(strict_types=1);

namespace Cassandra\Tests\Feature\DefinedTypes;

use Cassandra\Uuid;

$keyspace = 'user_defined_types';
$table = 'users';

beforeAll(function () use($keyspace, $table) {
    migrateKeyspace(<<<CQL
        CREATE KEYSPACE $keyspace WITH replication = {
            'class': 'SimpleStrategy',
            'replication_factor': 1
        };
        USE $keyspace;
        CREATE TYPE address (street text, city text, zip int);
        CREATE TYPE addresses (home frozen<address>, work frozen<address>);
        CREATE TABLE $table (
            id uuid PRIMARY KEY,
            name text,
            addresses frozen<addresses>
        );
    CQL
    );
});

afterAll(function () use($keyspace) {
    dropKeyspace($keyspace);
});

it('Using Cassandra user defined types from schema metadata', function () use($keyspace, $table) {

    $session = scyllaDbConnection($keyspace);

    $keyspace = $session->schema()->keyspace('user_defined_types');
    $addressType = $keyspace->userType("address");
    $addressesType = $keyspace->userType("addresses");

    $users = [
        [
            new Uuid('56357d2b-4586-433c-ad24-afa9918bc415'),
            'Arthur Canhassi',
            $addressesType->create(
                'home', $addressType->create(
                'city', 'Phoenix',
                'street', '9042 Cassandra Lane',
                'zip', 85023))
        ],
        [
            new Uuid('ce359590-8528-4682-a9f3-add53fc9aa09'),
            'Kevin Malone',
            $addressesType->create(
                'home', $addressType->create(
                'city', 'New York',
                'street', '1000 Database Road',
                'zip', 10025),
                'work', $addressType->create(
                'city', 'New York',
                'street', '60 SSTable Drive',
                'zip', 10024)
            )
        ],
    ];

    foreach ($users as $user) {
        $options = array('arguments' => $user);
        $session->execute("INSERT INTO users (id, name, addresses) VALUES (?, ?, ?)", $options);
    }

    $result = $session->execute("SELECT * FROM $table");

    $row = $result->first();
    expect($row['id'])
        ->toBeInstanceOf(Uuid::class)
        ->map(fn(Uuid $value) => (string)$value)
        ->toBe('56357d2b-4586-433c-ad24-afa9918bc415')
        ->and($row['name'])
        ->toBe('Arthur Canhassi');

    $addresses = $row['addresses'];
    expect($addresses)
        ->toHaveCount(2)
        ->and($addresses->values())
        ->toHaveCount(2)
        ->and($addresses->values()['home'])
        ->toHaveCount(3)
        ->and($addresses->values()['work'])
        ->toBeNull();

    $address = $addresses->values()['home']->values();
    expect($address)
        ->toHaveCount(3)
        ->and($address['street'])
        ->toBe('9042 Cassandra Lane')
        ->and($address['city'])
        ->toBe('Phoenix')
        ->and($address['zip'])
        ->toBe(85023);

    $row = $result->offsetGet(1);
    expect($row['id'])
        ->toBeInstanceOf(Uuid::class)
        ->map(fn(Uuid $value) => (string)$value)
        ->toBe('ce359590-8528-4682-a9f3-add53fc9aa09')
        ->and($row['name'])
        ->toBe('Kevin Malone');

    $addresses = $row['addresses'];
    expect($addresses)
        ->toHaveCount(2)
        ->and($addresses->values())
        ->toHaveCount(2)
        ->and($addresses->values()['home'])
        ->toHaveCount(3)
        ->and($addresses->values()['work'])
        ->toHaveCount(3);

    $address = $addresses->values()['home']->values();
    expect($address)
        ->toHaveCount(3)
        ->and($address['street'])
        ->toBe('1000 Database Road')
        ->and($address['city'])
        ->toBe('New York')
        ->and($address['zip'])
        ->toBe(10025);

    $workAddress = $addresses->values()['work']->values();
    expect($workAddress)
        ->toHaveCount(3)
        ->and($workAddress['street'])
        ->toBe('60 SSTable Drive')
        ->and($workAddress['city'])
        ->toBe('New York')
        ->and($workAddress['zip'])
        ->toBe(10024);
});
