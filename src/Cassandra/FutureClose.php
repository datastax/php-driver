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
 * A future returned from `Cassandra\Session::closeAsync()`
 * @see Cassandra\Session::closeAsync()
 */
final class FutureClose implements Future
{
    private $resource;
    private $session;
    private $resolved;

    public function __construct($resource, &$session)
    {
        $this->resource =  $resource;
        $this->session  =& $session;
        $this->resolved =  false;
    }

    public function get($timeout = null)
    {
        if ($this->resolved) {
            return;
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

        $this->resource = null;
        $this->session  = null;
        $this->resolved = true;
    }
}
