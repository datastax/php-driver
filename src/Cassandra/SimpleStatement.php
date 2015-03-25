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
    public function resource($consistency, $serialConsistency, $pageSize, array $arguments = null)
    {
        $count    = count($arguments);
        $resource = cassandra_statement_new($this->cql, $count);

        cassandra_statement_set_consistency($resource, $consistency);
        cassandra_statement_set_paging_size($resource, $pageSize);

        if (!is_null($serialConsistency)) {
          cassandra_statement_set_serial_consistency($resource, $serialConsistency);
        }

        if ((isset($arguments))) {
            foreach ($arguments as $name => $argument) {
                cassandra_statement_bind($resource, $name, $argument);
            }
        }

        return $resource;
    }
}
