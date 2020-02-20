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
 * A PHP representation of the CQL UDT datatype
 */
final class UserTypeValue implements Value, \Countable, \Iterator {

    /**
     * Creates a new user type value with the given name/type pairs.
     *
     * @param array $types Array of types
     */
    public function __construct($types) { }

    /**
     * The type of this user type value.
     *
     * @return \Cassandra\Type
     */
    public function type() { }

    /**
     * Array of values in this user type value.
     *
     * @return array values
     */
    public function values() { }

    /**
     * Sets the value at name in this user type value.
     *
     * @param mixed $value A value or null
     *
     * @return void
     */
    public function set($value) { }

    /**
     * Retrieves the value at a given name.
     *
     * @param string $name String of the field name
     *
     * @return mixed A value or null
     */
    public function get($name) { }

    /**
     * Total number of elements in this user type value.
     *
     * @return int count
     */
    public function count() { }

    /**
     * Current element for iteration
     *
     * @return mixed The current element
     */
    public function current() { }

    /**
     * Current key for iteration
     *
     * @return int current key
     */
    public function key() { }

    /**
     * Move internal iterator forward
     *
     * @return void
     */
    public function next() { }

    /**
     * Check whether a current value exists
     *
     * @return bool
     */
    public function valid() { }

    /**
     * Rewind internal iterator
     *
     * @return void
     */
    public function rewind() { }

}
