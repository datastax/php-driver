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
 * A PHP representation of the CQL `timeuuid` datatype
 */
final class Timeuuid
{
    /**
     * Creates a timeuuid from a given timestamp or current time.
     * @param int $timestamp Unix timestamp
     */
    public function __construct(int $timestamp = null) {}

    /**
     * Returns this timeuuid as string
     * @retval string timeuuid
     */
    public function string __toString() {}

    /**
     * Returns this timeuuid as string
     * @retval string timeuuid
     */
    public function string uuid() {}

    /**
     * Returns the version of this timeuuid
     * @retval int version of this timeuuid
     */
    public function int version() {}

    /**
     * Unix timestamp
     * @see time()
     * @retval int seconds
     */
    public function int time() {}

    /**
     * Converts current timeuuid to PHP DateTime
     * @retval DateTime
     */
    public function \DateTime toDateTime() {}
}
