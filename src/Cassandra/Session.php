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
 * A session is used to prepare and execute statements.
 *
 * @see Cassandra\Cluster::connect()
 */
interface Session
{
    /**
     * Executes a given statement and returns a result
     *
     * @param  Statement        $statement statement to be executed
     * @param  ExecutionOptions $options   execution options
     *
     * @throws Cassandra\Exception
     *
     * @return Result               execution result
     */
    function execute(Statement $statement, ExecutionOptions $options = null);

    /**
     * Executes a given statement and returns a future result
     *
     * Note that this method ignores ExecutionOptions::$timeout option, you can
     * provide one to Future::get() instead.
     *
     * @param  Statement $statement statement to be executed
     * @param  array     $options   execution options
     *
     * @return Cassandra\Future     future result
     */
    function executeAsync(Statement $statement, ExecutionOptions $options = null);

    /**
     * Creates a prepared statement from a given CQL string
     *
     * Note that this method only uses the ExecutionOptions::$timeout option,
     * all other options will be ignored.
     *
     * @param  string $cql        CQL statement string
     * @param  array  $options    execution options
     *
     * @throws Cassandra\Exception
     *
     * @return PreparedStatement  prepared statement
     */
    function prepare($cql, ExecutionOptions $options = null);

    /**
     * Asynchronously prepares a statement and returns a future prepared statement
     *
     * Note that all options passed to this method will be ignored.
     *
     * @param  string $cql       CQL string to be prepared
     * @param  array  $options   preparation options
     * @return Cassandra\Future  statement
     */
    function prepareAsync($cql, ExecutionOptions $options = null);

    /**
     * Closes current session and all of its connections
     *
     * @param float|null Timeout to wait for closure in seconds
     *
     * @return void
     */
    function close($timeout = null);

    /**
     * Asynchronously closes current session once all pending requests have finished
     *
     * @return Cassandra\Future  future
     */
    function closeAsync();
}
