<?php

/**
 * Copyright 2015-2016 DataStax, Inc.
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
 * Common interface implemented by all numeric types, providing basic
 * arithmetic functions.
 *
 * @see Bigint
 * @see Decimal
 * @see Float
 * @see Varint
 */
interface Numeric {

    /**
     * @param Cassandra\Numeric $num a number to add to this one
     * @return Numeric sum
     */
    public function add($num);

    /**
     * @param Cassandra\Numeric $num a number to subtract from this one
     * @return Numeric difference
     */
    public function sub($num);

    /**
     * @param Cassandra\Numeric $num a number to multiply this one by
     * @return Numeric product
     */
    public function mul($num);

    /**
     * @param Cassandra\Numeric $num a number to divide this one by
     * @return Numeric quotient
     */
    public function div($num);

    /**
     * @param Cassandra\Numeric $num a number to divide this one by
     * @return Numeric remainder
     */
    public function mod($num);

    /**
     * @return Numeric absolute value
     */
    public function abs();

    /**
     * @return Numeric negative value
     */
    public function neg();

    /**
     * @return Numeric square root
     */
    public function sqrt();

    /**
     * @return int this number as int
     */
    public function toInt();

    /**
     * @return float this number as float
     */
    public function toDouble();

}
