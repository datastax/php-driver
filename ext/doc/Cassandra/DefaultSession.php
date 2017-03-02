<?php

/**
 * Copyright 2017 DataStax, Inc.
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
 * {@inheritDoc}
 */
final class DefaultSession implements Session {

    /**
     * {@inheritDoc}
     *
     * @param string|Statement $statement {@inheritDoc}
     * @param array|ExecutionOptions|null $options {@inheritDoc}
     *
     * @return Rows {@inheritDoc}
     */
    public function execute($statement, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param string|Statement $statement {@inheritDoc}
     * @param array|ExecutionOptions|null $options {@inheritDoc}
     *
     * @return FutureRows {@inheritDoc}
     */
    public function executeAsync($statement, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param string $cql {@inheritDoc}
     * @param ExecutionOptions $options {@inheritDoc}
     *
     * @return PreparedStatement {@inheritDoc}
     */
    public function prepare($cql, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param string $cql {@inheritDoc}
     * @param ExecutionOptions $options {@inheritDoc}
     *
     * @return FuturePreparedStatement {@inheritDoc}
     */
    public function prepareAsync($cql, $options) { }

    /**
     * {@inheritDoc}
     *
     * @param double $timeout {@inheritDoc}
     *
     * @return null {@inheritDoc}
     */
    public function close($timeout) { }

    /**
     * {@inheritDoc}
     *
     * @return FutureClose {@inheritDoc}
     */
    public function closeAsync() { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function metrics() { }

    /**
     * {@inheritDoc}
     *
     * @return Schema {@inheritDoc}
     */
    public function schema() { }

}
