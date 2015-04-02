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

use Cassandra\Exception\LogicException;

/**
 * Default cluster implementation/
 * @see Cassandra\Cluster
 */
final class DefaultCluster implements Cluster
{
    /**
     * Cluster resource
     * @var resource
     */
    private $resource;

    /**
     * Default cluster-wide execution options
     * @var ExecutionOptions
     */
    private $defaults;

    /**
     * @access private
     * @param resource $resource Cluster resource
     */
    public function __construct($resource, ExecutionOptions $defaults)
    {
        $this->resource = $resource;
        $this->defaults = $defaults;
    }

    /**
     * {@inheritDoc}
     */
    public function connect($keyspace = null)
    {
        return $this->connectAsync($keyspace)->get();
    }

    public function connectAsync($keyspace = null)
    {
        $session = cassandra_session_new();

        if (is_null($keyspace)) {
            $future = cassandra_session_connect($session, $this->resource);
        } else {
            $future = cassandra_session_connect_keyspace($session, $this->resource, $keyspace);
        }

        return new FutureSession($future, new DefaultSession($session, $this->defaults));
    }
}
