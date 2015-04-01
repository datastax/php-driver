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

use Cassandra\Exception\LibraryException;
use Cassandra\Exception\InvalidArgumentException;
use Cassandra\Exception\TimeoutException;

/**
 * A future returned from `Cassandra\Session::prepareAsync()`
 * This future will resolve with a `Cassandra\PreparedStatement` or an exception.
 * @see Cassandra\Session::prepareAsync()
 */
final class FuturePreparedStatement implements Future
{
    /**
     * Actual future resource
     * @var resource
     */
    private $resource;
    /**
     * The prepared statement that this future will resolve with
     * @var Cassandra\PreparedStatement
     */
    private $statement;

    /**
     * Creates a new future prepared statement
     * @access private
     * @param  resource  $resource  actual future resource
     */
    public function __construct($resource)
    {
        $this->resource  = $resource;
        $this->statement = null;
    }

    /**
     * {@inheritDoc}
     */
    public function get($timeout = null)
    {
        if (!is_null($this->statement)) {
            return $this->statement;
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

        $this->statement = new PreparedStatement(cassandra_future_get_prepared($this->resource));
        $this->resource  = null;
        return $this->statement;
    }
}
