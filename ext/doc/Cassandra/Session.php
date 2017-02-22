<?php

/**
 * Copyright 2015-2016 DataStax, Inc.
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
 */
interface Session {

    /**
     * Execute a query.
     *
     * @throws Exception
     *
     * @param string|Statement $statement string or statement to be executed.
     * @param array|ExecutionOptions|null $options execution options (optional)
     * @return Rows A colleciton of rows.
     */
    public function execute($statement, $options);

    /**
     * Execute a query asyncrhonously. This method returns immediately, but
     * the query continues execution in the background.
     * @param string|Statement $statement string or statement to be executed.
     * @param array|ExecutionOptions|null $options execution options (optional)
     * @return FutureRows A future that can be used to retrieve the result.
     */
    public function executeAsync($statement, $options);

    /**
     * Prepare a query for execution.
     *
     * @throws Exception
     *
     * @param string $cql The query to be prepared.
     * @param Cassandra\ExecutionOptions $options Options to control preparing the query.
     * @return PreparedStatement A prepared statement that can be bound with parameters and executed.
     */
    public function prepare($cql, $options);

    /**
     * Asynchronously prepare a query for execution.
     * @param string $cql The query to be prepared.
     * @param Cassandra\ExecutionOptions $options Options to control preparing the query.
     * @return FuturePreparedStatement A future that can be used to retrieve the prepared statement.
     */
    public function prepareAsync($cql, $options);

    /**
     * Close the session and all its connections.
     *
     * @throws Exception
     *
     * @param double $timeout The amount of time in seconds to wait for the session to close.
     * @return null Nothing.
     */
    public function close($timeout);

    /**
     * Asynchronously close the session and all its connections.
     * @return FutureClose A future that can be waited on.
     */
    public function closeAsync();

    /**
     * Get performance and diagnostic metrics.
     * @return array Performance/Diagnostic metrics.
     */
    public function metrics();

    /**
     * Get a snapshot of the cluster's current schema.
     * @return Schema A snapshot of the cluster's schema.
     */
    public function schema();

}
