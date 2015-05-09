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

use Iterator;
use Countable;
use ArrayAccess;

/**
 * A PHP representation of the CQL `map` datatype
 */
final class Map implements Countable, IteratorAggregate, ArrayAccess
{
    /**
     * Creates a new map of a given key and value type
     * @param string $keyType   one of `Cassandra::TYPE_*`
     * @param string $valueType one of `Cassandra::TYPE_*`
     */
    public function __construct(string $keyType, string $valueType) {}

    /**
     * Returns key type of this map
     * @return string key type, one of `Cassandra::TYPE_*`
     */
    public function string keyType() {}

    /**
     * Returns all keys in the map as an array
     * @return array keys
     */
    public function array keys() {}

    /**
     * Returns value type of this map
     * @return string value type, one of `Cassandra::TYPE_*`
     */
    public function string valueType() {}

    /**
     * Returns all values in the map as an array
     * @return array values
     */
    public function array values() {}

    /**
     * Sets key/value in the map
     * @param mixed $key   key
     * @param mixed $value value
     */
    public function void set(mixed $key, mixed $value) {}

    /**
     * Gets the value of the key in the map
     * @param  mixed $key Key
     * @return mixed      Value or null
     */
    public function mixed get(mixed $key) {}

    /**
     * Removes the key from the map
     * @param  mixed   $key Key
     * @return bool      Whether the key was removed or not, e.g. didn't exist
     */
    public function bool remove(mixed $key) {}

    /**
     * Returns whether the key is in the map
     * @param  mixed   $key Key
     * @return bool      Whether the key is in the map or not
     */
    public function bool has(mixed $key) {}
}
