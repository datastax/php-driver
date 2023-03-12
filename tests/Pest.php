<?php

declare(strict_types=1);

use Cassandra\Session;
use Cassandra\Tests\Feature\TestCase;
use Symfony\Component\Process\Process;
use Symfony\Component\Process\Exception\ProcessFailedException;

uses(TestCase::class)->in('Feature');

function env(string $key, $default = null)
{
    $e = getenv($key);

    if ($e !== false) {
        return $e;
    }

    return $default;
}

function dropKeyspace(
    string $keyspace,
    ?array $hosts = [],
    ?int $port = null,
    ?string $username = null,
    ?string $password = null,
): void {
    $envHosts = env('SCYLLADB_HOSTS', $hosts);

    if (is_string($envHosts)) {
        $hosts = explode(',', $envHosts);
    }

    if (empty($hosts)) {
        $hosts = ['127.0.0.1'];
    }

    $process = new Process([
        'cqlsh',
        '-u',
        env('SCYLLADB_USERNAME', $username ?? 'cassandra'),
        '-p',
        env('SCYLLADB_PASSWORD', $password ?? 'cassandra'),
        '--execute',
        'DROP KEYSPACE IF EXISTS ' . $keyspace,
        $hosts[0],
        (int)env('SCYLLADB_PORT', $port ?? 9042),
    ]);

    $process->run();

    if (!$process->isSuccessful()) {
        throw new ProcessFailedException($process);
    }
}

function migrateKeyspace(
    string $schema,
    ?array $hosts = [],
    ?int $port = null,
    ?string $username = null,
    ?string $password = null,
): void {
    $envHosts = env('SCYLLADB_HOSTS', $hosts);

    if (is_string($envHosts)) {
        $hosts = explode(',', $envHosts);
    }

    if (empty($hosts)) {
        $hosts = ['127.0.0.1'];
    }

    $process = new Process([
        env('CQLSH_BINARY','cqlsh'),
        '-u',
        env('SCYLLADB_USERNAME', $username ?? 'cassandra'),
        '-p',
        env('SCYLLADB_PASSWORD', $password ?? 'cassandra'),
        '--execute',
        $schema,
        $hosts[0],
        (int)env('SCYLLADB_PORT', $port ?? 9042),
    ]);

    $process->run();

    if (!$process->isSuccessful()) {
//         throw new ProcessFailedException($process);
    }
}

function scyllaDbConnection(
    ?string $keyspace = null,
    ?array $hosts = [],
    ?int $port = null,
    ?string $username = null,
    ?string $password = null
): Session {
    $envHosts = env('SCYLLADB_HOSTS', $hosts);

    if (is_string($envHosts)) {
        $hosts = explode(',', $envHosts);
    }

    if (empty($hosts)) {
        $hosts = ['127.0.0.1'];
    }

    $builder = Cassandra::cluster()
        ->withContactPoints(...$hosts)
        ->withPort((int)env('SCYLLADB_PORT', $port ?? 9042))
        ->withCredentials(env('SCYLLADB_USERNAME', $username ?? 'cassandra'), env('SCYLLADB_USERNAME', $password ?? 'cassandra'))
        ->withPersistentSessions(true)
        ->withTokenAwareRouting(true)
        ->build();

    return $builder->connect(env('SCYLLADB_KEYSPACE', $keyspace ?? 'simplex'));
}


expect()->extend('map', function (Closure $closure) {
    return expect($closure->call($this, $this->value));
});