<?php

/**
 * Copyright 2015-2017 DataStax, Inc.
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

use PHPUnit\Framework\TestCase;

/**
 * @requires extension cassandra
 */
class VarintTest extends TestCase {
    public function testAddLarge()
    {
        $varint1 = new Varint("9223372036854775807");
        $varint2 = new Varint("1");
        $this->assertEquals("9223372036854775808", (string)$varint1->add($varint2));
    }

    public function testSubLarge()
    {
        $varint1 = new Varint("-9223372036854775808");
        $varint2 = new Varint("1");
        $this->assertEquals("-9223372036854775809", (string)$varint1->sub($varint2));
    }

    public function testMulLarge()
    {
        $varint1 = new Varint("9223372036854775807");
        $varint2 = new Varint("2");
        $this->assertEquals("18446744073709551614", (string)$varint1->mul($varint2));
    }

    public function testDivLarge()
    {
        $varint1 = new Varint("18446744073709551614");
        $varint2 = new Varint("9223372036854775807");
        $this->assertEquals(2, (int)$varint1->div($varint2));
    }

    public function testModLarge()
    {
        $varint1 = new Varint("1");
        $varint2 = new Varint("18446744073709551614");
        $this->assertEquals(1, (int)$varint1->mod($varint2));
    }

    public function testAbsLarge()
    {
        $varint1 = new Varint("-18446744073709551614");
        $this->assertEquals("18446744073709551614", (string)$varint1->abs());
    }

    public function testNegLarge()
    {
        $varint1 = new Varint("18446744073709551614");
        $this->assertEquals("-18446744073709551614", (string)$varint1->neg());
    }

    public function testSqrtLarge()
    {
        $varint1 = new Varint("340282366920938463389587631136930004996");
        $this->assertEquals("18446744073709551614", (string)$varint1->sqrt());
    }

    public function testCompareEqualsLarge()
    {
        $value1 = new Varint('123456789123456789123456789');
        $value2 = new Varint('123456789123456789123456789');
        $this->assertEquals($value1, $value2);
        $this->assertTrue($value1 == $value2);
    }

    public function testCompareNotEqualsLarge()
    {
        $value1 = new Varint('123456789123456789123456789');
        $value2 = new Varint('999999999999999999999999999');
        $this->assertNotEquals($value1, $value2);
        $this->assertFalse($value1 == $value2);
    }
}
