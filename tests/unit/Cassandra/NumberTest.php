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
use RangeException;

/**
 * @requires extension cassandra
 */
class NumberTest extends TestCase {
    /**
     * Minimum value for integer
     */
    const PHP_INT_MIN = -PHP_INT_MAX - 1;
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

    const LIMITS = array(
      "Cassandra\\Tinyint" => array (
        "min" => self::PHP_TINYINT_MIN,
        "max" => self::PHP_TINYINT_MAX
      ),
      "Cassandra\\Smallint" => array (
        "min" => self::PHP_SMALLINT_MIN,
        "max" => self::PHP_SMALLINT_MAX
      ),
      "Cassandra\\Bigint" => array (
        "min" => self::PHP_INT_MIN,
        "max" => PHP_INT_MAX
      )
    );

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
        $provider = array();
        foreach (self::LIMITS as $class => $limits) {
            $max = $limits["max"];

            // Integer values
            $provider[] = array($class, $max);

            // Double values
            $provider[] = array($class, (double) $max);

            // String values (in base 2, 8, 10, 16)
            foreach (array(2 => "0b", 8 => "0", 10 => "", 16 => "0x") as $base => $prefix) {
                $provider[] = array(
                    $class,
                    $prefix . base_convert((string) $max, 10, $base),
                    $max
                );
            }
        }
        return $provider;
    }

    /**
     * Data provider to use for high overflow values of a number data type
     *
     * @return array 1 + Maximum values for number types to use in data provider
     *     [
     *         [
     *             [0] => Class name for the number type
     *             [1] => Value to use for instantiating type/expected value
     *         ]
     *     ]
     */
    public function highOverflowValues()
    {
        $provider = array();
        foreach(self::LIMITS as $class => $limits) {
            $max = $limits["max"];

            // Integer values
            if ($class != "Cassandra\\Bigint") {
                $provider[] = array($class, $max + 1);
            }

            // Double values
            // Bigint is special because the double representation + 1 loses precision, so there's no overflow.
            // So instead, we multiply the double by 10 to make a significant difference. Also, on
            // 32-bit platforms, it's not possible to overflow, so don't try.

            if ($class == "Cassandra\\Bigint") {
                if (PHP_INT_MAX != (2 ** 31) - 1) {
                    $provider[] = array($class, ((double) $max) * 10);
                }
            } else {
                $provider[] = array($class, (double) $max + 1);
            }

            // String values (in base 2, 8, 10, 16)
            foreach (array(2 => "0b", 8 => "0", 10 => "", 16 => "0x") as $base => $prefix) {
                // Bigint is special because adding 1 to its max would overflow the 64-bit int.
                // Instead, we create a string rep of the max and add a 0 afterwards to effectively multiply
                // by 10. Again, if we're on a 32-bit platform, it's not possible to overflow Bigint.

                if ($class == "Cassandra\\Bigint" && PHP_INT_MAX == (2 ** 31) - 1) {
                    continue;
                }

                $val = ($class == "Cassandra\\Bigint") ?
                    $prefix . base_convert((string) $max, 10, $base) . "0" :
                    $prefix . base_convert((string) ($max + 1), 10, $base);
                $provider[] = array(
                    $class,
                    $val
                );
            }
        }

        // For Tinyint and Smallint, also test that supplying a value larger than a 32-bit value will
        // produce an error message complaining about the appropriate type's range, not the range of
        // a 32-bit int. This is relevant because the parsing function for these types actually parses
        // 32-bit string representations of numbers.

        $provider[] = array("Cassandra\Tinyint", "2147483648");
        $provider[] = array("Cassandra\Smallint", "2147483648");

        return $provider;
    }

    /**
     * Data provider to use for low overflow values of a number data type
     *
     * @return array Minimum values - 1 for number types to use in data provider
     *     [
     *         [
     *             [0] => Class name for the number type
     *             [1] => Value to use for instantiating type/expected value
     *         ]
     *     ]
     */
    public function lowOverflowValues()
    {
        $provider = array();
        foreach(self::LIMITS as $class => $limits) {
            $min = $limits["min"];

            // Integer values
            if ($class != "Cassandra\\Bigint") {
                $provider[] = array($class, $min - 1);
            }

            // Double values
            // Bigint is special because the double representation - 1 loses precision, so there's no overflow.
            // So instead, we multiply the double by 10 to make a significant difference. Also, on
            // 32-bit platforms, it's not possible to overflow, so don't try.


            if ($class == "Cassandra\\Bigint") {
                if (PHP_INT_MAX != (2 ** 31) - 1) {
                    $provider[] = array($class, ((double) $min) * 10);
                }
            } else {
                $provider[] = array($class, (double) $min - 1);
            }

            // String values (in base 2, 8, 10, 16)
            foreach (array(2 => "-0b", 8 => "-0", 10 => "-", 16 => "-0x") as $base => $prefix) {
                // Bigint is special because subtracting 1 from its min would overflow the 64-bit int.
                // Instead, we create a string rep of the min and add a 0 afterwards to effectively multiply
                // by 10. Again, if we're on a 32-bit platform, it's not possible to overflow Bigint.

                if ($class == "Cassandra\\Bigint" && PHP_INT_MAX == (2 ** 31) - 1) {
                    continue;
                }

                $strMin = (string) $min;
                if($strMin[0] === '-') {
                    $strMin = substr($strMin, 1);
                }
                $strMinMinusOne = (string) ($min - 1);
                if($strMinMinusOne[0] === '-') {
                    $strMinMinusOne = substr($strMinMinusOne, 1);
                }
                $val = ($class == "Cassandra\\Bigint") ?
                    $prefix . base_convert($strMin, 10, $base) . "0":
                    $prefix . base_convert($strMinMinusOne, 10, $base);
                $provider[] = array(
                    $class,
                    $val
                );
            }
        }

        // For Tinyint and Smallint, also test that supplying a value smaller than the min 32-bit value will
        // produce an error message complaining about the appropriate type's range, not the range of
        // a 32-bit int. This is relevant because the parsing function for these types actually parses
        // 32-bit string representations of numbers.

        $provider[] = array("Cassandra\Tinyint", "-2147483649");
        $provider[] = array("Cassandra\Smallint", "-2147483649");

        return $provider;
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
        $provider = array();
        foreach (self::LIMITS as $class => $limits) {
            $min = $limits["min"];

            // Integer values
            $provider[] = array($class, $min);

            // Double values
            $provider[] = array($class, (double) $min);

            // String values (in base 2, 8, 10, 16)
            foreach (array(2 => "-0b", 8 => "-0", 10 => "", 16 => "-0x") as $base => $prefix) {
                // For the 'max' cases, we did a base_convert even for base10, and it was fine.
                // However, for min (PHP_INT_MIN to be specific), base_convert trashes the value.
                // Since we don't need base conversion for base 10 and we have to special case something,
                // we just don't do base_convert for any of the base 10 tests.

                $strMin = (string) $min;
                if($strMin[0] === '-') {
                    $strMin = substr($strMin, 1);
                }
                $val = ($base == 10) ? $min : $prefix . base_convert($strMin, 10, $base);
                $provider[] = array(
                    $class,
                    $val,
                    $min
                );
            }
        }
        return $provider;
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
     */
    public function testVarintOverflowTooBig() {
        $this->expectException(Exception\RangeException::class);
        $this->expectExceptionMessage("Value is too big");
        $number = new Varint("9223372036854775808");
        $number->toInt();
    }

    /**
     * This Varint test will be valid for both 32-bit and 64-bit longs
     */
    public function testVarintOverflowTooSmall() {
        $this->expectException(Exception\RangeException::class);
        $this->expectExceptionMessage("Value is too small");
        $number = new Varint("-9223372036854775809");
        $number->toInt();
    }


    /**
     * @dataProvider numberClasses
     */
    public function testEmptyString($class) {
        $this->expectException(Exception\InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid integer value: ''");
        new $class("");
    }

    /**
     * @dataProvider numberClasses
     */
    public function testInvalidString($class) {
        $this->expectException(Exception\InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid integer value: 'invalid123'");
        new $class("invalid123");
    }

    /**
     * @dataProvider numberClasses
     */
    public function testInvalidCharacters($class) {
        $this->expectException(Exception\InvalidArgumentException::class);
        $this->expectExceptionMessageMatches("/Invalid characters were found in value: '123.123'|Invalid integer value: '123.123'/");
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
     */
    public function testMultiplyOutOfRange($class) {
        $this->expectException(Exception\RangeException::class);
        $this->expectExceptionMessage("Product is out of range");
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
     */
    public function testDivisionByZero($class) {
        $this->expectException(Exception\DivideByZeroException::class);
        $this->expectExceptionMessage("Cannot divide by zero");
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
     */
    public function testModuloByZero($class) {
        $this->expectException(Exception\DivideByZeroException::class);
        $this->expectExceptionMessage("Cannot modulo by zero");
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
     */
    public function testAbsoluteValueDatatypeMinimum($class) {
        $this->expectException(Exception\RangeException::class);
        $this->expectExceptionMessage("Value doesn't exist");
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
     */
    public function testSquareRootNegative($class) {
        $this->expectException(Exception\RangeException::class);
        $this->expectExceptionMessage("Cannot take a square root of a negative number");
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
     */
    public function testBigintToIntTooSmall() {
        $this->expectException(Exception\RangeException::class);
        $this->expectExceptionMessage("Value is too small");
        $number = new Bigint("-9223372036854775808");
        $number->toInt();
    }

    /**
     * This is a special case test; underlying c long is 32-bits. Bigint can
     * hold a 64-bit value and the toInt() method can only return a maximum
     * value of a 32-bit value.
     *
     * @depends testIs32bitLong
     */
    public function testBigintToIntTooLarge() {
        $this->expectException(Exception\RangeException::class);
        $this->expectExceptionMessage("Value is too big");
        $number = new Bigint("9223372036854775807");
        $number->toInt();
    }

    /**
     * @dataProvider highOverflowValues
     **/
    public function testOverflowTooBig($class, $value)
    {
        if (is_double($value)) {
            $this->expectException(RangeException::class);
            $this->expectExceptionMessage("value must be between " . self::LIMITS[$class]["min"] . " and " . self::LIMITS[$class]["max"]);
        } else {
            $this->expectException(RangeException::class);
            $this->expectExceptionMessage("value must be between " . self::LIMITS[$class]["min"] . " and " . self::LIMITS[$class]["max"] . ", " .
                $value . " given");
        }
        new $class($value);
    }

    /**
     * @dataProvider lowOverflowValues
     **/
    public function testOverflowTooSmall($class, $value)
    {
        if (is_double($value)) {
            $this->expectException(RangeException::class);
            $this->expectExceptionMessage("value must be between " . self::LIMITS[$class]["min"] . " and " . self::LIMITS[$class]["max"]);
        } else {
            $this->expectException(RangeException::class);
            $this->expectExceptionMessage("value must be between " . self::LIMITS[$class]["min"] . " and " . self::LIMITS[$class]["max"] . ", " .
                $value . " given");
        }
        new $class($value);
    }
}
