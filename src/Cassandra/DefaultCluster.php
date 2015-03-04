<?php

namespace Cassandra;

use Cassandra\DefaultSession;

final class DefaultCluster implements Cluster
{
    /**
     * Cluster resource
     * @var resource
     */
    private $resource;

    /**
     * @access private
     * @param resource $resource Cluster resource
     */
    public function __construct($resource)
    {
        $this->resource = $resource;
    }

    /**
     * @access private
     */
    public function __destruct()
    {
        cassandra_cluster_free($this->resource);
        $this->resource = null;
    }

    /**
     * {@inheritDoc}
     */
    public function connect($keyspace = null)
    {
        $session = cassandra_session_new();
        $future  = cassandra_session_connect($session, $this->resource);

        Future::wait($future);
        unset($future);

        return new DefaultSession($session);
    }

    public function close()
    {
        cassandra_cluster_free($this->resource);

        return $this;
    }
}
