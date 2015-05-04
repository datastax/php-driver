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
 * A PHP representation of the CQL `decimal` datatype
 *
 * The actual value of a decimal is `$value * pow(10, $scale * -1)`
 */
final class Decimal
{
    /**
     * Creates a decimal from a given decimal string:
     * <code>
     * $decimal = new Cassandra\Decimal("1313123123.234234234234234234123");
     *
     * $this->assertEquals(21, $decimal->scale());
     * $this->assertEquals("1313123123234234234234234234123", $decimal->value());
     * </code>
     * @param  string  $value  Any decimal string
     */
    public function __construct($value) {}

    /**
     * String representation of this decimal
     * @return string Decimal value
     */
    public function __toString() {}

    /**
     * Numeric value of this decimal as string
     * @return string Numeric value
     */
    public function value() {}

    /**
     * Scale of this decimal as int
     * @return int Scale
     */
    public function scale() {}
}
