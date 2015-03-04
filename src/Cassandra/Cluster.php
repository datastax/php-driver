<?php

namespace Cassandra;

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

    /**
     * Terminate the cluster instance and all of its sessions
     *
     * @return  Cassandra\Cluster  self
     */
    public function close();
}
