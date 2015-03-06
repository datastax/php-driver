<?php

namespace Cassandra;

final class DefaultSession implements Session
{
    private $resource;

    public function __construct($resource)
    {
        $this->resource = $resource;
    }

    public function close()
    {
        if (is_null($this->resource)) {
            throw new LogicException("Session is already closed");
        }

        cassandra_session_free($this->resource);
        $this->resource = null;
    }

    /**
     * {@inheritDoc}
     */
    public function execute(Statement $statement, array $options = array())
    {
        return $this->executeAsync($statement, $options)->result();
    }

    /**
     * {@inheritDoc}
     */
    public function executeAsync(Statement $statement, array $options = array())
    {
        if (is_null($this->resource)) {
            return new Future\Exception(new LogicException("Session is already closed"));
        }

        $future = cassandra_session_execute($this->resource, $statement->resource());

        return new Future($future);
    }

    /**
     * {@inheritDoc}
     */
    public function prepare($cql, array $options = array())
    {

    }

    /**
     * {@inheritDoc}
     */
    public function prepareAsync($cql, array $options = array())
    {
        if (is_null($this->resource)) {
            return new Future\Exception(new LogicException("Session is already closed"));
        }

        $future = cassandra_session_prepare($cql);

        return new Future($future);
    }
}
