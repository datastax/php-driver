<?php

namespace Cassandra;

/**
 * Prepared statements are faster to execute because the server doesn't need
 * to process statement's CQL during the execution.
 *
 * With token-awareness enabled in the driver, prepared statements are even
 * faster, because they are sent directly to replica nodes and avoid the extra
 * network hop.
 *
 * @see Cassandra\Session::prepare()
 */
final class PreparedStatement implements Statement
{
    /**
     * @access private
     */
    private $resource;

    /**
     * @access private
     */
    public function __construct($resource)
    {
        $this->resource = $resource;
    }

    /**
     * {@inheritDoc}
     */
    public function resource(array $arguments = null, $consistency = null, $serialConsistency = null, $pageSize = null)
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
