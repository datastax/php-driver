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

use Cassandra\Exception\DivideByZeroException;
use InvalidArgumentException;
use PHPUnit\Framework\TestCase;
use RangeException;

/**
 * @requires extension cassandra
 */
class FloatTest extends TestCase
{
    const EPSILON = 0.00001;

    public function testThrowsWhenCreatingFromEmpty()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid float value: ''");
        new Float("");
    }

    public function testThrowsWhenCreatingFromInvalid()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid float value: 'invalid'");
        new Float("invalid");
    }

    public function testThrowsWhenCreatingFromInvalidTrailingChars()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid characters were found in value: '123.123    '");
        new Float("123.123    ");
    }

    /**
     * @dataProvider      outOfRangeStrings
     */
    public function testThrowsWhenCreatingOutOfRange($string)
    {
        $this->expectException(RangeException::class);
        new Float($string);
    }

    public function outOfRangeStrings()
    {
        return array(
            array("10e308"),
            array("-10e308"),
        );
    }

    /**
     * @dataProvider validStrings
     */
    public function testCorrectlyParsesStrings($number, $expected)
    {
        $number = new Float($number);
        $this->assertEqualsWithDelta((float)$expected, (float)$number, self::EPSILON);
    }

    public function validStrings()
    {
        return array(
            array("123", 123),
            array("123.123", 123.123),
            array(".123", 0.123),
            array("0.123", 0.123),
            array("1e10", 1e10),
        );
    }

    /**
     * @dataProvider validNumbers
     */
    public function testFromNumbers($number)
    {
        $float = new Float($number);
        $this->assertEqualsWithDelta((float)$number, (float)$float, self::EPSILON);
        $this->assertEquals((int)$number, $float->toInt());
        $this->assertEqualsWithDelta((float)$number, (float)(string)$float, self::EPSILON);
    }

    public function validNumbers()
    {
        return array(
            array(0.123),
            array(123),
        );
    }

    public function testAdd()
    {
        $float1 = new Float("1");
        $float2 = new Float("0.5");
        $this->assertEqualsWithDelta(1.5, (float)$float1->add($float2), self::EPSILON);
    }

    public function testSub()
    {
        $float1 = new Float("1");
        $float2 = new Float("0.5");
        $this->assertEqualsWithDelta(0.5, (float)$float1->sub($float2), self::EPSILON);
    }

    public function testMul()
    {
        $float1 = new Float("2");
        $float2 = new Float("0.5");
        $this->assertEqualsWithDelta(1.0, (float)$float1->mul($float2), self::EPSILON);
    }

    public function testDiv()
    {
        $float1 = new Float("1");
        $float2 = new Float("0.5");
        $this->assertEqualsWithDelta(2, (float)$float1->div($float2), self::EPSILON);
    }

    public function testDivByZero()
    {
        $this->expectException(DivideByZeroException::class);
        $float1 = new Float("1");
        $float2 = new Float("0");
        $float1->div($float2);
    }

    public function testMod()
    {
        $float1 = new Float("1");
        $float2 = new Float("2");
        $this->assertEqualsWithDelta(1, (float)$float1->mod($float2), self::EPSILON);
    }

    public function testAbs()
    {
        $float1 = new Float("-123.123");
        $this->assertEqualsWithDelta(123.123, (float)$float1->abs(), self::EPSILON);
    }

    public function testNeg()
    {
        $float1 = new Float("123.123");
        $this->assertEqualsWithDelta(-123.123, (float)$float1->neg(), self::EPSILON);
    }

    public function testSqrt()
    {
        $float1 = new Float("4.0");
        $this->assertEqualsWithDelta(2.0, (float)$float1->sqrt(), self::EPSILON);
    }

    /**
     * @dataProvider equalTypes
     */
    public function testCompareEquals($value1, $value2)
    {
        $this->assertEquals($value1, $value2);
        $this->assertTrue($value1 == $value2);
    }

    public function equalTypes()
    {
        return array(
            array(new Float('3.14159'), new Float('3.14159')),
            array(new Float(1.1), new Float(1.1)),
        );
    }

    /**
     * @dataProvider notEqualTypes
     */
    public function testCompareNotEquals($value1, $value2)
    {
        $this->assertNotEquals($value1, $value2);
        $this->assertFalse($value1 == $value2);
    }

    public function notEqualTypes()
    {
        return array(
            array(new Float('3.14159'), new Float('3.1415')),
            array(new Float(1.1), new Float(2.2)),
        );
    }
}
