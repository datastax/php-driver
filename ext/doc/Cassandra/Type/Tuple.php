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
 * A class that represents the tuple type. The tuple type is able to represent
 * a composite type of one or more types accessed by index.
 */
final class Tuple extends \Cassandra\Type {

    private function __construct() { }

    /**
     * Returns "tuple"
     *
     * @return string "tuple"
     */
    public function name() { }

    /**
     * Returns type representation in CQL, e.g. `tuple<varchar, int>`
     *
     * @return string Type representation in CQL
     */
    public function __toString() { }

    /**
     * Returns types of values
     *
     * @return array An array of types
     */
    public function types() { }

    /**
     * Creates a new Tuple from the given values. When no values given,
     * creates a tuple with null for the values.
     *
     * @param mixed $values ,... One or more values to be added to the tuple.
     *
     * @throws Exception\InvalidArgumentException when values given are of a
     *                                            different type than what the
     *                                            tuple expects.
     *
     * @return \Cassandra\Tuple A tuple with given values.
     */
    public function create($values) { }

}
