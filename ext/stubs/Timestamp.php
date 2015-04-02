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
 * A PHP representation of the CQL `timestamp` datatype
 */
final class Timestamp
{
    /**
     * Creates a new timestamp from either unix timestamp and microseconds or
     * from the current time by default.
     * @param int $time Unix timestamp
     * @param int $usec Microseconds
     */
    public function __construct($time = null, $usec = null) {}

    /**
     * Unix timestamp
     * @see time
     * @return int seconds
     */
    public function time() {}

    /**
     * Microtime from this timestamp
     * @see microtime
     * @param  boolean      $get_as_float Whther to get this value as float
     * @return float|string               Float or string representation
     */
    public function microtime($get_as_float = false) {}

    /**
     * Converts current timestamp to PHP DateTime
     * @return DateTime
     */
    public function toDateTime() {}

    /**
     * Returns a string representation of this timestamp
     * @return string timestamp
     */
    public function __toString() {}
}
