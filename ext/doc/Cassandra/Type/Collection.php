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
 * A class that represents the list type. The list type contains the type of the
 * elements contain in the list.
 */
final class Collection extends \Cassandra\Type {

    private function __construct() { }

    /**
     * Returns "list"
     *
     * @return string "list"
     */
    public function name() { }

    /**
     * Returns type of values
     *
     * @return \Cassandra\Type Type of values
     */
    public function valueType() { }

    /**
     * Returns type representation in CQL, e.g. `list<varchar>`
     *
     * @return string Type representation in CQL
     */
    public function __toString() { }

    /**
     * Creates a new Collection from the given values.  When no values
     * given, creates an empty list.
     *
     * @param mixed $value ,...      One or more values to be added to the list.
     *
     * @throws Exception\InvalidArgumentException when values given are of a
     *                                            different type than what this
     *                                            list type expects.
     *
     * @return \Cassandra\Collection A list with given values.
     */
    public function create($value) { }

}
