<?php

namespace Cassandra;

final class ExecutionOptions
{
    /**
     * Consistency level for the request.
     * Must be one of Cassandra::CONSISTENCY_*
     * @var int
     */
    public $consistency;

    /**
     * Serial consistency to be used for conditional updates.
     * Must be Cassandra::CONSISTENCY_SERIAL or Cassandra::CONSISTENCY_LOCAL_SERIAL
     * @var int
     */
    public $serialConsistency;

    /**
     * Result page size.
     * Must be a positive integer or exactly -1 to disable paging.
     * @var int
     */
    public $pageSize;

    /**
     * Maximum wait time for the request in seconds.
     * @var int
     */
    public $timeout;

    /**
     * A list or map of positional or named arguments for the statement.
     * Note that named parameters are only supported for prepared statements.
     * @var array
     */
    public $arguments;
}
