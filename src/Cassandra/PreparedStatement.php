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

/**
 * Prepared statements are faster to execute because the server doesn't need
 * to process statement's CQL during the execution.
 *
 * With token-awareness enabled in the driver, prepared statements are even
 * faster, because they are sent directly to replica nodes and avoid the extra
 * network hop.
 *
 * @see Cassandra\Session::prepare()
 */
final class PreparedStatement implements Statement
{
    /**
     * Actual prepared statement resource
     * @var resource
     */
    private $resource;

    /**
     * Creates a new prepared statement
     * @access private
     * @param resource $resource actual prepared statement resource
     */
    public function __construct($resource)
    {
        $this->resource = $resource;
    }

    /**
     * {@inheritDoc}
     */
    public function resource(array $arguments = null, $consistency = null, $serialConsistency = null, $pageSize = null)
    {
        $resource = cassandra_prepared_bind($this->resource);

        cassandra_statement_set_consistency($resource, $consistency);
        cassandra_statement_set_paging_size($resource, $pageSize);

        if (!is_null($serialConsistency)) {
          cassandra_statement_set_serial_consistency($resource, $serialConsistency);
        }

        if ((isset($arguments))) {
            foreach ($arguments as $name => $argument) {
                cassandra_statement_bind($resource, $name, $argument);
            }
        }

        return $resource;
    }
}
