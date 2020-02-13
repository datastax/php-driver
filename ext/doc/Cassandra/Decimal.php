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
 * A PHP representation of the CQL `decimal` datatype
 *
 * The actual value of a decimal is `$value * pow(10, $scale * -1)`
 */
final class Decimal implements Value, Numeric {

    /**
     * Creates a decimal from a given decimal string:
     *
     * ~~~{.php}
     * <?php
     * $decimal = new Cassandra::Decimal("1313123123.234234234234234234123");
     * ~~~
     *
     * @param string $value Any decimal string
     */
    public function __construct($value) { }

    /**
     * String representation of this decimal.
     *
     * @return string Decimal value
     */
    public function __toString() { }

    /**
     * The type of this decimal.
     *
     * @return \Cassandra\Type
     */
    public function type() { }

    /**
     * Numeric value of this decimal as string.
     *
     * @return string Numeric value
     */
    public function value() { }

    /**
     * Scale of this decimal as int.
     *
     * @return int Scale
     */
    public function scale() { }

    /**
     * @param \Cassandra\Numeric $num a number to add to this one
     * @return \Cassandra\Numeric sum
     */
    public function add($num) { }

    /**
     * @param \Cassandra\Numeric $num a number to subtract from this one
     * @return \Cassandra\Numeric difference
     */
    public function sub($num) { }

    /**
     * @param \Cassandra\Numeric $num a number to multiply this one by
     * @return \Cassandra\Numeric product
     */
    public function mul($num) { }

    /**
     * @param \Cassandra\Numeric $num a number to divide this one by
     * @return \Cassandra\Numeric quotient
     */
    public function div($num) { }

    /**
     * @param \Cassandra\Numeric $num a number to divide this one by
     * @return \Cassandra\Numeric remainder
     */
    public function mod($num) { }

    /**
     * @return \Cassandra\Numeric absolute value
     */
    public function abs() { }

    /**
     * @return \Cassandra\Numeric negative value
     */
    public function neg() { }

    /**
     * @return \Cassandra\Numeric square root
     */
    public function sqrt() { }

    /**
     * @return int this number as int
     */
    public function toInt() { }

    /**
     * @return float this number as float
     */
    public function toDouble() { }

}
