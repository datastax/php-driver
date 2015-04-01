<?php

/**
 * Copyright 2015 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace Cassandra;

use Cassandra\Exception\InvalidArgumentException;

/**
 * Batch statements are used to execute a series of simple or prepared statements.
 * There are 3 types of batch statements:
 *  * `Cassandra::BATCH_LOGGED`   - this is the default batch type. This batch
 *     gurantees that either all or none of its statements will be executed.
 *     This behavior is achieved by writing a batch log on the coordinator, which
 *     slows down the execution somewhat.
 *   * `Cassandra::BATCH_UNLOGGED` - this batch will not be verified when executed,
 *     which makes it faster than a `LOGGED` batch, but means that some of its statements
 *     might fail, while others - succeed.
 *   * `Cassandra::BATCH_COUNTER`  - this batch is used for counter updates, which
 *     are, unlike other writes, not idempotent.
 *
 * @see Cassandra::BATCH_LOGGED
 * @see Cassandra::BATCH_UNLOGGED
 * @see Cassandra::BATCH_COUNTER
 */
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

    /**
     * Adds a statement to this batch
     * @param Cassandra\Statement $statement the statement to add
     * @param array|null          $arguments positional or named arguments
     */
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

    /**
     * {@inheritDoc}
     */
    public function resource(array $arguments = null, $consistency = null, $serialConsistency = null, $pageSize = null)
    {
        if (!is_null($arguments)) {
            throw new InvalidArgumentException(
                "Batch statements do not allow arguments, specify them for " .
                "each individual statement when calling BatchStatement::add()"
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
