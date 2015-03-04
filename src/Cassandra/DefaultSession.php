<?php

namespace Cassandra;

final class DefaultSession implements Session
{
    private $resource;

    public function __construct($resource)
    {
        $this->resource = $resource;
    }

    public function __destruct()
    {
        cassandra_session_free($this->resource);
        $this->resource = null;
    }

    /**
     * {@inheritDoc}
     */
    public function execute(Statement $statement, array $options = array())
    {
        
    }

    /**
     * {@inheritDoc}
     */
    public function prepare($cql, array $options = array())
    {

    }
}
