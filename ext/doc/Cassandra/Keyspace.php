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
interface Keyspace {

    /**
     * Returns keyspace name
     *
     * @return string Name
     */
    public function name();

    /**
     * Returns replication class name
     *
     * @return string Replication class
     */
    public function replicationClassName();

    /**
     * Returns replication options
     *
     * @return \Cassandra\Map Replication options
     */
    public function replicationOptions();

    /**
     * Returns whether the keyspace has durable writes enabled
     *
     * @return string Whether durable writes are enabled
     */
    public function hasDurableWrites();

    /**
     * Returns a table by name
     *
     * @param string $name Table name
     *
     * @return \Cassandra\Table|null Table instance or null
     */
    public function table($name);

    /**
     * Returns all tables defined in this keyspace
     *
     * @return array An array of `Table` instances
     */
    public function tables();

    /**
     * Get user type by name
     *
     * @param string $name User type name
     *
     * @return \Cassandra\Type\UserType|null A user type or null
     */
    public function userType($name);

    /**
     * Get all user types
     *
     * @return array An array of user types
     */
    public function userTypes();

    /**
     * Get materialized view by name
     *
     * @param string $name Materialized view name
     *
     * @return \Cassandra\MaterizedView|null A materialized view or null
     */
    public function materializedView($name);

    /**
     * Gets all materialized views
     *
     * @return array An array of materialized views
     */
    public function materializedViews();

    /**
     * Get a function by name and signature
     *
     * @param string $name Function name
     * @param string|\Cassandra\Type $params Function arguments
     *
     * @return \Cassandra\Function|null A function or null
     */
    public function function_($name, ...$params);

    /**
     * Get all functions
     *
     * @return array An array of functions
     */
    public function functions();

    /**
     * Get an aggregate by name and signature
     *
     * @param string $name Aggregate name
     * @param string|\Cassandra\Type $params Aggregate arguments
     *
     * @return \Cassandra\Aggregate|null An aggregate or null
     */
    public function aggregate($name, ...$params);

    /**
     * Get all aggregates
     *
     * @return array An array of aggregates
     */
    public function aggregates();

}
