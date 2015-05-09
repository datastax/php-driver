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
use Cassandra\Exception\LogicException;

/**
 * Actual session implementation.
 *
 * @see Cassandra::Session
 */
final class DefaultSession implements Session
{
    /**
     * {@inheritDoc}
     */
    public function Rows execute(Statement $statement, ExecutionOptions $options = null) {}

    /**
     * {@inheritDoc}
     */
    public function Future executeAsync(Statement $statement, ExecutionOptions $options = null) {}

    /**
     * {@inheritDoc}
     */
    public function PreparedStatement prepare(string $cql, ExecutionOptions $options = null) {}

    /**
     * {@inheritDoc}
     */
    public function Future prepareAsync(string $cql, ExecutionOptions $options = null) {}

    /**
     * {@inheritDoc}
     */
    public function void close(float $timeout = null) {}

    /**
     * {@inheritDoc}
     */
    public function Future closeAsync() {}
}
