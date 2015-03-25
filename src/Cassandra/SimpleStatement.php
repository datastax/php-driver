<?php

namespace Cassandra;

use Cassandra\Value;

/**
 * Simple statements can be executed using a Session instance.
 * They are constructed with a CQL string that can contain positional
 * argument markers `?`.
 *
 * @see Cassandra\Session::execute()
 */
final class SimpleStatement implements Statement
{
    /**
     * @access private
     */
    private $cql;

    /**
     * Creates a new simple statement with the provided CQL
     * @param string $cql CQL string for this simple statement
     */
    public function __construct($cql)
    {
        $this->cql = $cql;
    }

    /**
     * {@inheritDoc}
     */
    public function resource(array $arguments = null, $consistency = null, $serialConsistency = null, $pageSize = null)
    {
        $count    = count($arguments);
        $resource = cassandra_statement_new($this->cql, $count);

        cassandra_statement_set_consistency($resource, $consistency);
        cassandra_statement_set_paging_size($resource, $pageSize);

        if (!is_null($serialConsistency)) {
            cassandra_statement_set_serial_consistency($resource, $serialConsistency);
        }

        if (!is_null($arguments)) {
            foreach ($arguments as $name => $argument) {
                cassandra_statement_bind($resource, $name, $argument);
            }
        }

        return $resource;
    }
}
