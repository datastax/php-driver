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
use Cassandra\Exception\LogicException;

/**
 * Actual session implementation
 * @see Cassandra\Session
 */
final class DefaultSession implements Session
{
    private $resource;
    private $defaults;

    public function __construct($resource, ExecutionOptions $defaults)
    {
        $this->resource = $resource;
        $this->defaults = $defaults;
    }

    /**
     * {@inheritDoc}
     */
    public function execute(Statement $statement, ExecutionOptions $options = null)
    {
        if (is_null($this->resource)) {
            throw new LogicException("Session is already closed");
        }

        $timeout = $this->defaults->timeout;

        if ($options && isset($options->timeout)) {
            if (!(is_numeric($options->timeout) || $options->timeout > 0)) {
                throw new InvalidArgumentException(sprintf(
                    "Timeout must be positive number, %s given",
                    var_export($options->timeout, true)
                ));
            }

            $timeout = $options->timeout;
        }

        return $this->executeAsync($statement, $options)->get($timeout);
    }

    /**
     * {@inheritDoc}
     */
    public function executeAsync(Statement $statement, ExecutionOptions $options = null)
    {
        if (is_null($this->resource)) {
            return new FutureException(new LogicException("Session is already closed"));
        }

        $consistency       = $this->defaults->consistency;
        $serialConsistency = $this->defaults->serialConsistency;
        $pageSize          = $this->defaults->pageSize;
        $arguments         = null;

        if ($options) {
            if (isset($options->consistency)) {
                if (!in_array($options->consistency, array(
                        \Cassandra::CONSISTENCY_ANY,
                        \Cassandra::CONSISTENCY_ONE,
                        \Cassandra::CONSISTENCY_TWO,
                        \Cassandra::CONSISTENCY_THREE,
                        \Cassandra::CONSISTENCY_QUORUM,
                        \Cassandra::CONSISTENCY_ALL,
                        \Cassandra::CONSISTENCY_LOCAL_QUORUM,
                        \Cassandra::CONSISTENCY_EACH_QUORUM,
                        \Cassandra::CONSISTENCY_SERIAL,
                        \Cassandra::CONSISTENCY_LOCAL_SERIAL,
                        \Cassandra::CONSISTENCY_LOCAL_ONE))) {
                    return new FutureException(new InvalidArgumentException(sprintf(
                        "Invalid consistency, must be one of " .
                        "Cassandra::CONSISTENCY_*, %s given",
                        var_export($options->consistency, true)
                    )));
                }

                $consistency = $options->consistency;
            }

            if (isset($options->serialConsistency)) {
                if (!in_array($options->serialConsistency, array(
                        \Cassandra::CONSISTENCY_SERIAL,
                        \Cassandra::CONSISTENCY_LOCAL_SERIAL))) {
                    return new FutureException(new InvalidArgumentException(sprintf(
                        "Invalid serial consistency, must be " .
                        "Cassandra::CONSISTENCY_SERIAL or " .
                        "Cassandra::CONSISTENCY_LOCAL_SERIAL, %s given",
                        var_export($options->serialConsistency, true)
                    )));
                }

                $serialConsistency = $options->serialConsistency;
            }

            if (isset($options->pageSize)) {
                if (!(is_numeric($options->pageSize) &&
                    ($options->pageSize === -1 || $options->pageSize > 0))) {
                    return new FutureException(new InvalidArgumentException(sprintf(
                        "Page size must be a positive integer or exactly -1, %s given",
                        var_export($options->pageSize, true)
                    )));
                }

                $pageSize = $options->pageSize;
            }

            if (isset($options->arguments)) {
                if (!is_array($options->arguments)) {
                    return new FutureException(new InvalidArgumentException(sprintf(
                        "Arguments must be an array, %s given",
                        var_export($options->arguments, true)
                    )));
                }

                $arguments = $options->arguments;
            }
        }

        try {
            $resource = $statement->resource($arguments, $consistency, $serialConsistency, $pageSize);
        } catch (Exception $e) {
            return new FutureException($e);
        }

        if ($statement instanceOf BatchStatement) {
            $future = cassandra_session_execute_batch($this->resource, $resource);
        } else {
            $future = cassandra_session_execute($this->resource, $resource);
        }

        return new FutureRows($future, $this->resource, $resource);
    }

    /**
     * {@inheritDoc}
     */
    public function prepare($cql, ExecutionOptions $options = null)
    {
        if (is_null($this->resource)) {
            throw new LogicException("Session is already closed");
        }

        $timeout = $this->defaults->timeout;

        if ($options && isset($options->timeout)) {
            if (!(is_numeric($options->timeout) || $options->timeout > 0)) {
                throw new InvalidArgumentException(sprintf(
                    "Timeout must be positive number, %s given",
                    var_export($options->timeout, true)
                ));
            }

            $timeout = $options->timeout;
        }

        return $this->prepareAsync($cql, $options)->get($timeout);
    }

    /**
     * {@inheritDoc}
     */
    public function prepareAsync($cql, ExecutionOptions $options = null)
    {
        if (is_null($this->resource)) {
            return new FutureException(new LogicException("Session is already closed"));
        }

        return new FuturePreparedStatement(cassandra_session_prepare(
            $this->resource, (string) $cql
        ));
    }

    public function close($timeout = null)
    {
        if (is_null($this->resource)) {
            throw new LogicException("Session is already closed");
        }

        $this->closeAsync()->get($timeout);
    }

    public function closeAsync()
    {
        if (is_null($this->resource)) {
            return new FutureException(new LogicException("Session is already closed"));
        }

        return new FutureClose(
            cassandra_session_close($this->resource),
            $this->resource
        );
    }
}
