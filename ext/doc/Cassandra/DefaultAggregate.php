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
 * A PHP representation of an aggregate
 */
final class DefaultAggregate implements Aggregate {

    /**
     * Returns the full name of the aggregate
     *
     * @return string Full name of the aggregate including name and types
     */
    public function name() { }

    /**
     * Returns the simple name of the aggregate
     *
     * @return string Simple name of the aggregate
     */
    public function simpleName() { }

    /**
     * Returns the argument types of the aggregate
     *
     * @return array Argument types of the aggregate
     */
    public function argumentTypes() { }

    /**
     * Returns the state function of the aggregate
     *
     * @return \Cassandra\Function State public function of the aggregate
     */
    public function stateFunction() { }

    /**
     * Returns the final function of the aggregate
     *
     * @return \Cassandra\Function Final public function of the aggregate
     */
    public function finalFunction() { }

    /**
     * Returns the initial condition of the aggregate
     *
     * @return \Cassandra\Value
     */
    public function initialCondition() { }

    /**
     * Returns the state type of the aggregate
     *
     * @return \Cassandra\Type State type of the aggregate
     */
    public function stateType() { }

    /**
     * Returns the return type of the aggregate
     *
     * @return \Cassandra\Type Return type of the aggregate
     */
    public function returnType() { }

    /**
     * Returns the signature of the aggregate
     *
     * @return string Signature of the aggregate (same as name())
     */
    public function signature() { }

}
