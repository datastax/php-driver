<?php

namespace Cassandra;

use Cassandra\DefaultSession;
use Cassandra\Exception\LogicException;

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
     * {@inheritDoc}
     */
    public function connect($keyspace = null)
    {
        if (is_null($this->resource)) {
            throw new LogicException("Cluster is already closed");
        }

        $session = cassandra_session_new();

        if (is_null($keyspace)) {
            $future = cassandra_session_connect($session, $this->resource);
        } else {
            $future = cassandra_session_connect_keyspace($session, $this->resource, $keyspace);
        }

        $code = cassandra_future_error_code($future);

        if ($code === 0) {
            return new DefaultSession($session);
        }

        Util::throwException($code, cassandra_future_error_message($future));
    }

    /**
     * {@inheritDoc}
     */
    public function close()
    {
        if (is_null($this->resource)) {
            throw new LogicException("Cluster is already closed");
        }

        cassandra_cluster_free($this->resource);
        $this->resource = null;

        return $this;
    }
}
