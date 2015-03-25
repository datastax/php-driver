<?php

namespace Cassandra;

use Cassandra\Exception\LogicException;

final class DefaultCluster implements Cluster
{
    /**
     * Cluster resource
     * @var resource
     */
    private $resource;

    /**
     * Default cluster-wide execution options
     * @var ExecutionOptions
     */
    private $defaults;

    /**
     * Holds a reference to SSLContext resource if was configured with SSL
     * @var resource|null
     */
    private $ssl;

    /**
     * @access private
     * @param resource $resource Cluster resource
     */
    public function __construct($resource, ExecutionOptions $defaults, $ssl = null)
    {
        $this->resource = $resource;
        $this->defaults = $defaults;
        $this->ssl      = $ssl;
    }

    /**
     * {@inheritDoc}
     */
    public function connect($keyspace = null)
    {
        return $this->connectAsync($keyspace)->get();
    }

    public function connectAsync($keyspace = null)
    {
        $session = cassandra_session_new();

        if (is_null($keyspace)) {
            $future = cassandra_session_connect($session, $this->resource);
        } else {
            $future = cassandra_session_connect_keyspace($session, $this->resource, $keyspace);
        }

        return new FutureSession($future, new DefaultSession($session, $this->defaults));
    }
}
