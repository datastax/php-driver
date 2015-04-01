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

/**
 * A PHP representation of the CQL `varint` datatype
 */
final class Varint
{
    /**
     * Creates a new variable length integer
     * @param string $value integer value as a string
     */
    public function __construct($value) {}

    /**
     * Returns the integer value
     * @return string integer value
     */
    public function __toString() {}

    /**
     * Returns the integer value
     * @return string integer value
     */
    public function value() {}
}
