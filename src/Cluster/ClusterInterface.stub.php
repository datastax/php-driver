<?php

/** @generate-class-entries */

namespace Cassandra {
    interface Cluster {
        public function connect(?string $keyspace = null, ?int $timeout = null): Session;
        public function connectAsync(?string $keyspace = null): Future;
    }
}