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
 * A PHP representation of the CQL `blob` datatype
 */
final class Blob
{
    /**
     * Creates a new bytes array
     * @param string $bytes any bytes
     */
    public function __construct($bytes) {}

    /**
     * Returns bytes as a hex string
     * @return string bytes as hexadecimal string
     */
    public function bytes() {}

    /**
     * Returns bytes as a hex string
     * @return string bytes as hexadecimal string
     */
    public function __toString() {}
    /*
    public function toAscii();
    public function toText();
    public function toVarchar();
    public function toBigint();
    public function toCounter();
    public function toInt();
    public function toVarint();
    public function toBoolean();
    public function toDecimal();
    public function toDouble();
    public function toFloat();
    public function toInet();
    public function toTimestamp();
    public function toUuid();
    public function toTimeuuid();
    */
}
