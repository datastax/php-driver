<?php

declare(strict_types=1);

use Cassandra\Cluster;

$builder = Cassandra::cluster()
        ->withContactPoints('127.0.0.1')
        ->withPort(9042)
        ->withCredentials('cassandra', 'cassandra')
        ->withPersistentSessions(true)
        ->withTokenAwareRouting(true)
        ->build();


$session = $builder->connect();