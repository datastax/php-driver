<?php

namespace Cassandra;

final class PreparedStatement implements Statement
{
    private $resource;

    public function __construct($resource)
    {
        $this->resource = $resource;
    }

    public function resource($consistency, $serialConsistency, $pageSize, array $arguments = null)
    {
        $resource = cassandra_prepared_bind($this->resource);

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
