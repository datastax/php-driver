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
 * This future results is resolved with `Cassandra\Rows`.
 * @see Cassandra\Session::executeAsync()
 */
final class FutureRows implements Future
{
    /**
     * Result future resource
     * @var resource
     */
    private $resource;

    /**
     * Session resource
     * @var resource
     */
    private $session;

    /**
     * Statement resource
     * @var resource
     */
    private $statement;

    /**
     * Rows that this future will resolve with
     * @var Cassandra\Rows
     */
    private $rows;

    /**
     * Creates new rows future.
     * @access private
     * @param resource $resource  actual future resource
     * @param resource $session   a session resource (used for paging)
     * @param resource $statement a statement resource (used for paging)
     */
    public function __construct($resource, $session, $statement)
    {
        $this->resource  = $resource;
        $this->session   = $session;
        $this->statement = $statement;
        $this->rows      = null;
    }

    /**
     * {@inheritDoc}
     */
    public function get($timeout = null)
    {
        if (!is_null($this->rows)) {
            return $this->rows;
        }

        if (is_null($timeout)) {
            cassandra_future_wait($this->resource);
        } elseif (!is_numeric($timeout) || $timeout <= 0) {
            throw new InvalidArgumentException(sprintf(
                "Timeout must be positive number, %s given",
                var_export($timeout, true)
            ));
        } else {
            cassandra_future_wait_timed($this->resource, $timeout);
        }

        $this->rows = new Rows(
                          cassandra_future_get_result($this->resource),
                          $this->session, $this->statement
                      );
        $this->resource  = null;
        $this->session   = null;
        $this->statement = null;

        return $this->rows;
    }
}
