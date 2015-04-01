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

use Cassandra\Value;

/**
 * Simple statements can be executed using a Session instance.
 * They are constructed with a CQL string that can contain positional
 * argument markers `?`.
 *
 * @see Cassandra\Session::execute()
 */
final class SimpleStatement implements Statement
{
    /**
     * @access private
     */
    private $cql;

    /**
     * Creates a new simple statement with the provided CQL
     * @param string $cql CQL string for this simple statement
     */
    public function __construct($cql)
    {
        $this->cql = $cql;
    }

    /**
     * {@inheritDoc}
     */
    public function resource(array $arguments = null, $consistency = null, $serialConsistency = null, $pageSize = null)
    {
        $count    = count($arguments);
        $resource = cassandra_statement_new($this->cql, $count);

        cassandra_statement_set_consistency($resource, $consistency);
        cassandra_statement_set_paging_size($resource, $pageSize);

        if (!is_null($serialConsistency)) {
            cassandra_statement_set_serial_consistency($resource, $serialConsistency);
        }

        if (!is_null($arguments)) {
            foreach ($arguments as $name => $argument) {
                cassandra_statement_bind($resource, $name, $argument);
            }
        }

        return $resource;
    }
}
