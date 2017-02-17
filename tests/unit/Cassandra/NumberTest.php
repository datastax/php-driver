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
 * @requires extension cassandra
 */
class NumberTest extends \PHPUnit_Framework_TestCase {
    /**
     * Minimum value for integer
     */
    const PHP_INT_MIN = -(PHP_INT_MAX - 1);
    /**
     * Maximum value for tinyint
     */
    const PHP_TINYINT_MAX = (2 ** 7) - 1;
    /**
     * Minimum value for tinyint
     */
    const PHP_TINYINT_MIN = -(2 ** 7);
    /**
     * Maximum value for smallint
     */
    const PHP_SMALLINT_MAX = (2 ** 15) - 1;
    /**
     * Minimum value for smallint
     */
    const PHP_SMALLINT_MIN = -(2 ** 15);

    /**
     * Data provider to use for most tests involving number data types
     *
     * @return array Class names to test against
     *     [
     *         [0] => Class name for the number type
     *     ]
     */
    public function numberClasses() {
        return array(
            array("Cassandra\\Tinyint"),
            array("Cassandra\\Smallint"),
            array("Cassandra\\Bigint"),
            array("Cassandra\\Varint")
        );
    }

    /**
     * Data provider to use for valid number test cases
     *
     * @return array Class name and values to test against
     *     [
     *         [0] => Class name for the number type
     *         [1] => Valid value (also expected value)
     *         [2] => (Optional) Expected value
     *     ]
     */
    public function validNumbers() {
        // Array of valid numbers (and expected values)
        $values = array (
            // Integer values
            array(0),
            array(1),
            array(63),
            array(-1),
            array(-63),

            // Double values
            array(0.123, 0),
            array(0.0, 0),
            array(1.23, 1),
            array(1.9, 1),
            array(-1.23, -1),
            array(-1.9, -1)
        );

        // Iterate over all the types to create the final data provider
        $provider = array();
        foreach ($this->numberClasses() as $class) {
            foreach ($values as $value) {
                $data = array();
                $data[] = $class[0];
                $data[] = $value[0];
                if (count($value) > 1) {
                    $data[] = $value[1];
                }
                $provider[] = $data;
            }
        }
        return $provider;
    }

    /**
     * Data provider to use for valid number test cases
     *
     * @return array Class names to test against
     *     [
     *         [0] => Class name for the number type
     *         [1] => Valid value (also expected value)
     *         [2] => (Optional) Expected value
     *     ]
     */
    public function validStrings() {
        // Array of valid numbers (and expected values)
        $values = array (
            // Base 2, 8, 10, and 16 values
            array("0b0", 0),
            array("0b100101", 37),
            array("00", 0),
            array("045", 37),
            array("0", 0),
            array("45", 45),
            array("0x0", 0),
            array("0x37", 55),
            array(1),
            array(123),
            array("-0b100101", -37),
            array("-045", -37),
            array("-45", -45),
            array("-0x37", -55)
        );

        // Iterate over all the types to create the final data provider
        $provider = array();
        foreach ($this->numberClasses() as $class) {
            foreach ($values as $value) {
                $data = array();
                $data[] = $class[0];
                $data[] = $value[0];
                if (count($value) > 1) {
                    $data[] = $value[1];
                }
                $provider[] = $data;
            }
        }
        return $provider;
    }

    /**
     * Data provider to use for maximum values of a number data type
     *
     * @return array Maximum values for number types to use in data provider
     *     [
     *         [
     *             [0] => Class name for the number type
     *             [1] => Value to use for instantiating type/expected value
     *             [2] => (Optional) Expected value (e.g. non-base 10 values)
     *         ]
     *     ]
     */
    public function maximumValues()
    {
        return array(
            // Integer values
            array(
                "Cassandra\\Tinyint",
                self::PHP_TINYINT_MAX
            ),
            array(
                "Cassandra\\Smallint",
                self::PHP_SMALLINT_MAX
            ),
            array(
                "Cassandra\\Bigint",
                PHP_INT_MAX
            ),

            // Double values
            array(
                "Cassandra\\Tinyint",
                (double) self::PHP_TINYINT_MAX
            ),
            array(
                "Cassandra\\Smallint",
                (double) self::PHP_SMALLINT_MAX
            ),
            array(
                "Cassandra\\Bigint",
                (double) PHP_INT_MAX
            ),

            // String values (base 2)
            array(
                "Cassandra\\Tinyint",
                "0b" . base_convert((string) self::PHP_TINYINT_MAX, 10, 2),
                self::PHP_TINYINT_MAX
            ),
            array(
                "Cassandra\\Smallint",
                "0b" . base_convert((string) self::PHP_SMALLINT_MAX, 10, 2),
                self::PHP_SMALLINT_MAX
            ),
            array(
                "Cassandra\\Bigint",
                "0b" . base_convert((string) PHP_INT_MAX, 10, 2),
                PHP_INT_MAX
            ),

            // String values (base 8)
            array(
                "Cassandra\\Tinyint",
                "0" . base_convert((string) self::PHP_TINYINT_MAX, 10, 8),
                self::PHP_TINYINT_MAX
            ),
            array(
                "Cassandra\\Smallint",
                "0" . base_convert((string) self::PHP_SMALLINT_MAX, 10, 8),
                self::PHP_SMALLINT_MAX
            ),
            array(
                "Cassandra\\Bigint",
                "0" . base_convert((string) PHP_INT_MAX, 10, 8),
                PHP_INT_MAX
            ),

            // String values (base 10)
            array(
                "Cassandra\\Tinyint",
                (string) self::PHP_TINYINT_MAX,
                self::PHP_TINYINT_MAX
            ),
            array(
                "Cassandra\\Smallint",
                (string) self::PHP_SMALLINT_MAX,
                self::PHP_SMALLINT_MAX
            ),
            array(
                "Cassandra\\Bigint",
                (string) PHP_INT_MAX,
                PHP_INT_MAX
            ),

            // String values (base 16)
            array(
                "Cassandra\\Tinyint",
                "0x" . base_convert((string) self::PHP_TINYINT_MAX, 10, 16),
                self::PHP_TINYINT_MAX
            ),
            array(
                "Cassandra\\Smallint",
                "0x" . base_convert((string) self::PHP_SMALLINT_MAX, 10, 16),
                self::PHP_SMALLINT_MAX
            ),
            array(
                "Cassandra\\Bigint",
                "0x" . base_convert((string) PHP_INT_MAX, 10, 16),
                PHP_INT_MAX
            )
        );
    }

    /**
     * Data provider to use for minimum values of a number data type
     *
     * @return array Minimum values for number types to use in data provider
     *     [
     *         [
     *             [0] => Class name for the number type
     *             [1] => Value to use for instantiating type/expected value
     *             [2] => (Optional) Expected value (e.g. non-base 10 values)
     *         ]
     *     ]
     */
    public function minimumValues()
    {
        return array(
            // Integer values
            array(
                "Cassandra\\Tinyint",
                self::PHP_TINYINT_MIN
            ),
            array(
                "Cassandra\\Smallint",
                self::PHP_SMALLINT_MIN
            ),
            array(
                "Cassandra\\Bigint",
                self::PHP_INT_MIN
            ),

            // Double values
            array(
                "Cassandra\\Tinyint",
                (double) self::PHP_TINYINT_MIN
            ),
            array(
                "Cassandra\\Smallint",
                (double) self::PHP_SMALLINT_MIN
            ),
            array(
                "Cassandra\\Bigint",
                (double) self::PHP_INT_MIN
            ),

            // String values (base 2)
            array(
                "Cassandra\\Tinyint",
                "-0b" . base_convert((string) self::PHP_TINYINT_MIN, 10, 2),
                self::PHP_TINYINT_MIN
            ),
            array(
                "Cassandra\\Smallint",
                "-0b" . base_convert((string) self::PHP_SMALLINT_MIN, 10, 2),
                self::PHP_SMALLINT_MIN
            ),
            array(
                "Cassandra\\Bigint",
                "-0b" . base_convert((string) self::PHP_INT_MIN, 10, 2),
                self::PHP_INT_MIN
            ),

            // String values (base 8)
            array(
                "Cassandra\\Tinyint",
                "-0" . base_convert((string) self::PHP_TINYINT_MIN, 10, 8),
                self::PHP_TINYINT_MIN
            ),
            array(
                "Cassandra\\Smallint",
                "-0" . base_convert((string) self::PHP_SMALLINT_MIN, 10, 8),
                self::PHP_SMALLINT_MIN
            ),
            array(
                "Cassandra\\Bigint",
                "-0" . base_convert((string) self::PHP_INT_MIN, 10, 8),
                self::PHP_INT_MIN
            ),

            // String values (base 10)
            array(
                "Cassandra\\Tinyint",
                (string) self::PHP_TINYINT_MIN,
                self::PHP_TINYINT_MIN
            ),
            array(
                "Cassandra\\Smallint",
                (string) self::PHP_SMALLINT_MIN,
                self::PHP_SMALLINT_MIN
            ),
            array(
                "Cassandra\\Bigint",
                (string) self::PHP_INT_MIN,
                self::PHP_INT_MIN
            ),

            // String values (base 16)
            array(
                "Cassandra\\Tinyint",
                "-0x" . base_convert((string) self::PHP_TINYINT_MIN, 10, 16),
                self::PHP_TINYINT_MIN
            ),
            array(
                "Cassandra\\Smallint",
                "-0x" . base_convert((string) self::PHP_SMALLINT_MIN, 10, 16),
                self::PHP_SMALLINT_MIN
            ),
            array(
                "Cassandra\\Bigint",
                "-0x" . base_convert((string) self::PHP_INT_MIN, 10, 16),
                self::PHP_INT_MIN
            )
        );
    }

    /**
     * Dependency check to determine if extra testing functions should be
     * executed under platforms that have an underlying 32-bit long
     */
    public function testIs32bitLong() {
        if (PHP_INT_MAX == (2 ** 31) - 1) {
            $this->assertTrue(true, "Executing 32-bit specific test");
        } else {
            $this->markTestSkipped("Long is 64-bit for PHP v" . PHP_VERSION);
        }
    }

    /**
     * Assert the value (int, double, and string)
     *
     * @param mixed $expected Excepted value (will be converted to double or string)
     * @param mixed $number Value to assert
     */
    protected function assertValue($expected, $number) {
        $this->assertEquals((int) $expected, $number->toInt());
        $this->assertEquals((double)(int) $expected, $number->toDouble());
        $this->assertEquals((string)(int) $expected, (string) $number);
    }

    /**
     * This Varint test will be valid for both 32-bit and 64-bit longs
     *
     * @expectedException Cassandra\Exception\RangeException
     * @expectedExceptionMessage Value is too big
     */
    public function testVarintOverflowTooBig() {
        $number = new Varint("9223372036854775808");
        $number->toInt();
    }

    /**
     * This Varint test will be valid for both 32-bit and 64-bit longs
     *
     * @expectedException Cassandra\Exception\RangeException
     * @expectedExceptionMessage Value is too small
     */
    public function testVarintOverflowTooSmall() {
        $number = new Varint("-9223372036854775809");
        $number->toInt();
    }


    /**
     * @dataProvider numberClasses
     * @expectedException Cassandra\Exception\InvalidArgumentException
     * @expectedExceptionMessage Invalid integer value: ''
     */
    public function testEmptyString($class) {
        new $class("");
    }

    /**
     * @dataProvider numberClasses
     * @expectedException Cassandra\Exception\InvalidArgumentException
     * @expectedExceptionMessage Invalid integer value: 'invalid123'
     */
    public function testInvalidString($class) {
        new $class("invalid123");
    }

    /**
     * @dataProvider numberClasses
     * @expectedException Cassandra\Exception\InvalidArgumentException
     * @expectedExceptionMessageRegExp /Invalid characters were found in value: '123.123'|Invalid integer value: '123.123'/
     */
    public function testInvalidCharacters($class) {
        new $class("123.123");
    }

    /**
     * @dataProvider validNumbers
     */
    public function testFromNumbers($class, $value, $expected = null) {
        $number = new $class($value);
        if (is_null($expected)) {
            $expected = $value;
        }
        $this->assertValue($expected, $number);
    }

    /**
     * @dataProvider numberClasses
     */
    public function testAdd($class) {
        $value1 = new $class(1);
        $value2 = new $class(2);
        $this->assertValue(3, $value1->add($value2));
        $this->assertValue(3, $value2->add($value1));
    }

    /**
     * @dataProvider numberClasses
     */
    public function testSubtract($class) {
        $value1 = new $class(1);
        $value2 = new $class(2);
        $this->assertValue(-1, $value1->sub($value2));
        $this->assertValue(1, $value2->sub($value1));
    }

    /**
     * @dataProvider numberClasses
     */
    public function testMultiply($class) {
        $value1 = new $class(1);
        $value2 = new $class(2);
        $this->assertValue(2, $value1->mul($value2));
        $this->assertValue(2, $value2->mul($value1));
    }

    /**
     * @dataProvider numberClasses
     * @expectedException Cassandra\Exception\RangeException
     * @expectedExceptionMessage Product is out of range
     */
    public function testMultiplyOutOfRange($class) {
        if ($class == "Cassandra\\Bigint" ||
            $class == "Cassandra\\Varint") {
            $this->markTestSkipped("{$class} is not compatible with this test");
        }

        $value1 = call_user_func(array($class, "max"));
        $value2 = new $class(2);
        $value1->mul($value2);
    }

    /**
     * @dataProvider numberClasses
     */
    public function testDivision($class) {
        $value1 = new $class(1);
        $value2 = new $class(2);
        $this->assertValue(0, $value1->div($value2));
        $this->assertValue(2, $value2->div($value1));
    }

    /**
     * @dataProvider numberClasses
     * @expectedException Cassandra\Exception\DivideByZeroException
     * @expectedExceptionMessage Cannot divide by zero
     */
    public function testDivisionByZero($class) {
        $value1 = new $class(1);
        $value2 = new $class(0);
        $value1->div($value2);
    }

    /**
     * @dataProvider numberClasses
     */
    public function testModulo($class) {
        $value1 = new $class(1);
        $value2 = new $class(2);
        $this->assertValue(1, $value1->mod($value2));
        $this->assertValue(0, $value2->mod($value1));
    }

    /**
     * @dataProvider numberClasses
     * @expectedException Cassandra\Exception\DivideByZeroException
     * @expectedExceptionMessage Cannot modulo by zero
     */
    public function testModuloByZero($class) {
        $value1 = new $class(1);
        $value2 = new $class(0);
        $value1->mod($value2);
    }

    /**
     * @dataProvider numberClasses
     */
    public function testAbsoluteValue($class) {
        $number = new $class(-1);
        $this->assertValue(1, $number->abs());
    }

    /**
     * @dataProvider numberClasses
     * @expectedException Cassandra\Exception\RangeException
     * @expectedExceptionMessage Value doesn't exist
     */
    public function testAbsoluteValueDatatypeMinimum($class) {
        if ($class == "Cassandra\\Varint") {
            $this->markTestSkipped("{$class} is not compatible with this test");
        }
        $number = call_user_func(array($class, "min"));
        $number->abs();
    }

    /**
     * @dataProvider numberClasses
     */
    public function testNegation($class) {
        $number = new $class(1);
        $this->assertValue(-1, $number->neg());
    }

    /**
     * @dataProvider numberClasses
     */
    public function testSquareRoot($class) {
        $number = new $class(0);
        $this->assertValue(0, $number->sqrt());
        $number = new $class(4);
        $this->assertValue(2, $number->sqrt());

        // Test for truncated square root
        $number = new $class(3);
        $this->assertValue(1, $number->sqrt());
    }

    /**
     * @dataProvider numberClasses
     * @expectedException Cassandra\Exception\RangeException
     * @expectedExceptionMessage Cannot take a square root of a negative number
     */
    public function testSquareRootNegative($class) {
        $number = new $class(-1);
        $number->sqrt();
    }

    /**
     * @dataProvider numberClasses
     */
    public function testEquality($class) {
        $number = new $class("37");
        $equal = new $class(37);
        $notEqual = new $class(-37);

        $this->assertEquals($number, $equal);
        $this->assertTrue($number == $equal);
        $this->assertFalse($number != $equal);

        $this->assertNotEquals($number, $notEqual);
        $this->assertTrue($number != $notEqual);
        $this->assertFalse($number == $notEqual);
    }

    /**
     * @dataProvider validStrings
     */
    public function testFromString($class, $value, $expected = null) {
        $number = new $class($value);
        if (is_null($expected)) {
            $expected = $value;
        }
        $this->assertValue($expected, $number);
    }

    /**
     * @dataProvider maximumValues
     **/
    public function testMaximumValues($class, $value, $expected = null) {
        $number = new $class($value);
        if (is_null($expected)) {
            $expected = $value;
        }
        $this->assertValue($expected, $number);
    }

    /**
     * @dataProvider minimumValues
     **/
    public function testMinimumValues($class, $value, $expected = null) {
        $number = new $class($value);
        if (is_null($expected)) {
            $expected = $value;
        }
        $this->assertValue($expected, $number);
    }

    /**
     * This is a special case test; underlying c long is 32-bits. Bigint can
     * hold a 64-bit value and the toInt() method can only return a minimum
     * value of a 32-bit value.
     *
     * @depends testIs32bitLong
     * @expectedException Cassandra\Exception\RangeException
     * @expectedExceptionMessage Value is too small
     */
    public function testBigintToIntTooSmall() {
        $number = new Bigint("-9223372036854775808");
        $number->toInt();
    }

    /**
     * This is a special case test; underlying c long is 32-bits. Bigint can
     * hold a 64-bit value and the toInt() method can only return a maximum
     * value of a 32-bit value.
     *
     * @depends testIs32bitLong
     * @expectedException Cassandra\Exception\RangeException
     * @expectedExceptionMessage Value is too big
     */
    public function testBigintToIntTooLarge() {
        $number = new Bigint("9223372036854775807");
        $number->toInt();
    }
}
