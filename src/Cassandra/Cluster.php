<?php

namespace Cassandra;

/**
 * Cluster object is used to create Sessions
 */
interface Cluster
{
    /**
     * Creates a new Session instance
     *
     * @param  string  $keyspace  Optional keyspace name
     *
     * @return  Cassandra\Session  Session instance
     */
    public function connect($keyspace = null);
}
