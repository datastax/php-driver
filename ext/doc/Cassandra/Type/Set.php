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

namespace Cassandra\Type;

/**
 * A class that represents the set type. The set type contains the type of the
 * elements contain in the set.
 */
final class Set extends \Cassandra\Type {

    private function __construct() { }

    /**
     * Returns "set"
     *
     * @return string "set"
     */
    public function name() { }

    /**
     * Returns type of values
     *
     * @return \Cassandra\Type Type of values
     */
    public function valueType() { }

    /**
     * Returns type representation in CQL, e.g. `set<varchar>`
     *
     * @return string Type representation in CQL
     */
    public function __toString() { }

    /**
     * Creates a new Set from the given values.
     *
     * @param mixed $value ,... One or more values to be added to the set. When no values are given, creates an empty set.
     *
     * @throws Exception\InvalidArgumentException when values given are of a
     *                                            different type than what this
     *                                            set type expects.
     *
     * @return \Cassandra\Set A set with given values.
     */
    public function create($value) { }

}
