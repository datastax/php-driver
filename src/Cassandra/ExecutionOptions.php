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
 * Request execution options.
 *
 * @see Cassandra\Session::execute()
 * @see Cassandra\Session::executeAsync()
 * @see Cassandra\Session::prepare()
 * @see Cassandra\Session::prepareAsync()
 */
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

    public function __construct(array $options = null)
    {
        if (!is_null($options)) {
            foreach ($options as $key => $value) {
                switch ($key) {
                    case 'consistency':
                        $this->consistency = $value;
                        break;
                    case 'serial_consistency':
                        $this->serialConsistency = $value;
                        break;
                    case 'page_size':
                        $this->pageSize = $value;
                        break;
                    case 'timeout':
                        $this->timeout = $value;
                        break;
                    case 'arguments':
                        $this->arguments = $value;
                        break;
                    default:
                        throw new InvalidArgumentException(sprintf(
                            "Unexpected option '%s', supported options are: " .
                            "'consistency', 'serial_consistency', 'page_size', " .
                            "'timeout' and 'arguments'", $key
                        ));
                }
            }
        }
    }
}
