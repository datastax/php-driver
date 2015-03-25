<?php

namespace Cassandra;

use Cassandra\Exception\InvalidArgumentException;

final class BatchStatement implements Statement
{
    /**
     * The type of this statement, one of Cassandra::BATCH_*
     * @var int
     */
    private $type;

    /**
     * A collection of statements and their arguments
     * @var array
     */
    private $statements;

    /**
     * Creates a new batch statement
     * @param int $type must be one of Cassandra::BATCH_*
     */
    public function __construct($type = \Cassandra::BATCH_LOGGED)
    {
        if (!in_array($type, array(
                \Cassandra::BATCH_LOGGED,
                \Cassandra::BATCH_UNLOGGED,
                \Cassandra::BATCH_COUNTER
            ))) {
            throw new InvalidArgumentException(sprintf(
                "Invalid batch type, must be one of " .
                "Cassandra::BATCH_*, %s given",
                var_export($type, true)
            ));
        }
        $this->type       = $type;
        $this->statements = array();
    }

    public function add(Statement $statement, array $arguments = null)
    {
        if (!($statement instanceOf SimpleStatement
           || $statement instanceOf PreparedStatement)) {
            throw new InvalidArgumentException(sprintf(
                "Batch statements can contain only simple and prepared " .
                "statements, %s given", var_export($statement, true)
            ));
        }

        $part = new \stdClass();
        $part->statement = $statement;
        $part->arguments = $arguments;

        $this->statements[] = $part;

        return $this;
    }

    public function resource(array $arguments = null, $consistency = null, $serialConsistency = null, $pageSize = null)
    {
        if (!is_null($arguments)) {
            throw new InvalidArgumentException(
                "Batch statement does not allow arguments, specify them for " .
                "each individual statement using BatchStatement::add()"
            );
        }

        $resource = cassandra_batch_new($this->type);

        cassandra_batch_set_consistency($resource, $consistency);

        foreach ($this->statements as $part) {
            cassandra_batch_add_statement($resource, $part->statement->resource($part->arguments));
        }

        return $resource;
    }
}
