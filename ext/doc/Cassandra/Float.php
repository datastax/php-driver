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
 * A PHP representation of the CQL `float` datatype
 */
final class Float
{
    /**
     * Creates a new float
     * @param string $value float value as a string
     */
    public function __construct($value) {}

    /**
     * Returns the float value
     * @return string float value
     */
    public function value() {}

    /**
     * Returns string representation of the float value
     * @return string float value
     */
    public function __toString() {}
}
