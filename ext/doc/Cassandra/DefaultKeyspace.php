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
 * A PHP representation of a keyspace
 */
final class DefaultKeyspace implements Keyspace {

    /**
     * {@inheritDoc}
     *
     * @return string Name
     */
    public function name() { }

    /**
     * {@inheritDoc}
     *
     * @return string Replication class
     */
    public function replicationClassName() { }

    /**
     * {@inheritDoc}
     *
     * @return Map Replication options
     */
    public function replicationOptions() { }

    /**
     * {@inheritDoc}
     *
     * @return string Whether durable writes are enabled
     */
    public function hasDurableWrites() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name Table name
     *
     * @return Table Table instance or null
     */
    public function table($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array An array of `Table` instances
     */
    public function tables() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     *
     * @return Type\UserType|null {@inheritDoc}
     */
    public function userType($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function userTypes() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     *
     * @return MaterizedView|null {@inheritDoc}
     */
    public function materializedView($name) { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function materializedViews() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     * @param string|Type $params {@inheritDoc}
     *
     * @return Function|null {@inheritDoc}
     */
    public function function_($name, ...$params) { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function functions() { }

    /**
     * {@inheritDoc}
     *
     * @param string $name {@inheritDoc}
     * @param string|Type $params {@inheritDoc}
     *
     * @return Aggregate|null {@inheritDoc}
     */
    public function aggregate($name, ...$params) { }

    /**
     * {@inheritDoc}
     *
     * @return array {@inheritDoc}
     */
    public function aggregates() { }

}
