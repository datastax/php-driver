<?php

namespace Cassandra;

use Cassandra\Value;

final class SimpleStatement implements Statement
{
    private $resource;

    public function __construct($cql, array $arguments = null)
    {
        $count     = count($arguments);
        $statement = cassandra_statement_new($cql, $count);

        if ($count > 0) {
            foreach ($arguments as $i => $value) {
                if ($argument instanceOf Value) {
                    throw new InvalidArgumentException(sprintf(
                        "Unsupported value: %s", var_export($value, true)
                    ));
                }

                cassandra_statement_bind($statement, $value, $i);
            }
        }

        $this->resource = $statement;
    }

    /**
     * {@inheritDoc}
     */
    public function resource()
    {
        return $this->resource;
    }
}
