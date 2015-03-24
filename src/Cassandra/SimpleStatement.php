<?php

namespace Cassandra;

use Cassandra\Value;

final class SimpleStatement implements Statement
{
    private $cql;

    public function __construct($cql)
    {
        $this->cql = $cql;
    }

    /**
     * {@inheritDoc}
     */
    public function resource(array $arguments = null)
    {
        $count    = count($arguments);
        $resource = cassandra_statement_new($this->cql, $count);

        if ((isset($arguments))) {
            foreach ($arguments as $name => $argument) {
                cassandra_statement_bind($resource, $name, $argument);
            }
        }

        return $resource;
    }
}
