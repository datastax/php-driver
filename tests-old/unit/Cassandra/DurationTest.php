<?php

/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 */

namespace Cassandra;

use BadFunctionCallException;
use InvalidArgumentException;
use PHPUnit\Framework\TestCase;
use RangeException;
use RuntimeException;

/**
 * @requires extension cassandra
 */
class DurationTest extends TestCase
{
    public function testMonthsArgWrongType()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("months must be a long, a double, a numeric string or a Cassandra\Bigint, 1 given");
        new Duration(true, 2, 3);
    }

    public function testDaysArgWrongType()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("days must be a long, a double, a numeric string or a Cassandra\Bigint, 1 given");
        new Duration(1, true, 3);
    }

    public function testNanosArgWrongType()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("nanos must be a long, a double, a numeric string or a Cassandra\Bigint, 1 given");
        new Duration(1, 2, true);
    }

    public function testStringArgParseError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid integer value: 'ab'");
        new Duration(1, 2, "ab");
    }

    public function testString64BitArgOverflowError()
    {
        $this->expectException(RuntimeException::class);
        $this->expectExceptionMessage("value must be between -9223372036854775808 and 9223372036854775807, 9223372036854775808 given");
        new Duration(1, 2, "9223372036854775808");
    }

    public function testString64BitArgUnderflowError()
    {
        $this->expectException(RangeException::class);
        $this->expectExceptionMessage("value must be between -9223372036854775808 and 9223372036854775807, -9223372036854775809 given");
        new Duration(1, 2, "-9223372036854775809");
    }

    public function testDouble64BitArgOverflowError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("nanos must be between -9223372036854775808 and 9223372036854775807, 1.84467e+19 given");
        new Duration(1, 2, pow(2, 64));
    }

    public function testDouble64BitArgUnderflowError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("nanos must be between -9223372036854775808 and 9223372036854775807, -1.84467e+19 given");
        new Duration(1, 2, -pow(2, 64));
    }

    public function testString32BitArgOverflowError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("days must be between -2147483648 and 2147483647, 2147483648 given");
        new Duration(1, "2147483648", 0);
    }

    public function testString32BitArgUnderflowError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("days must be between -2147483648 and 2147483647, -2147483649 given");
        new Duration(1, "-2147483649", 0);
    }

    public function testLong32BitArgOverflowError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessageMatches("/days must be between -2147483648 and 2147483647, 8\.?58993.* given/");
        new Duration(1, 8589934592, 2);
    }

    public function testLong32BitArgUnderflowError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessageMatches("/days must be between -2147483648 and 2147483647, -8\.?58993.* given/");
        new Duration(1, -8589934592, 2);
    }

    public function testDouble32BitArgOverflowError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("months must be between -2147483648 and 2147483647, 8.58993e+9 given");
        new Duration(8589934592.5, 1, 2);
    }

    public function testDouble32BitArgUnderflowError()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("months must be between -2147483648 and 2147483647, -8.58993e+9 given");
        new Duration(-8589934592.5, 1, 2);
    }

    /**
     * @dataProvider mixedSigns
     */
    public function testMixedSignError($months, $days, $nanos)
    {
        $this->expectException(BadFunctionCallException::class);
        $this->expectExceptionMessage("A duration must have all non-negative or non-positive attributes");
        new Duration($months, $days, $nanos);
    }

    public function mixedSigns()
    {
        return array(
            array(1, 2, -3),
            array(1, -2, -3),
            array(1, -2, 3),
            array(-1, 2, 3),
            array(-1, 2, -3),
            array(-1, -2, 3)
        );
    }

    /**
     * @dataProvider durationStrings
     */
    public function testToString($months, $days, $nanos, $expected)
    {
        $duration = new Duration($months, $days, $nanos);
        $this->assertEquals($expected, $duration->__toString());
    }

    public function durationStrings()
    {
        return array(
            array(1, 2.5, 3, "1mo2d3ns"),
            array(-1.0, -2, -3, "-1mo2d3ns"),
            array(0, -2, -3, "-0mo2d3ns"),
        );
    }

    /**
     * @dataProvider goodAttributes
     */
    public function testAccessors($months, $days, $nanos)
    {
        $duration = new Duration($months, $days, $nanos);
        $this->assertEquals((int) $months, $duration->months());
        $this->assertEquals((int) $days, $duration->days());
        $this->assertEquals((string)$nanos, $duration->nanos());
        $this->assertEquals("duration", $duration->type()->name());
    }

    public function goodAttributes()
    {
        return array(
            array(1, 2.5, "5"),
            array(1, 2, 0),
            array(1, 0, 2),
            array(0, 1, 3),
            array(0, -1, -3),
            array(-1, 0, -3),
            array(-1, -1, 0),
            array(2147483647, 2147483647.0, new Bigint("9223372036854775807")),
            array(-2147483648, -2147483648.0, new Bigint("-9223372036854775808")),
            array("2147483647", 0, 0),
            array("-2147483648", 0, 0)
        );
    }

    /**
     * @dataProvider comparisonTable
     */
    public function testCompare($left, $right, $expected)
    {
        // When comparing, do it both ways to verify that the comparison operator is stable.
        $this->assertEquals($expected, $this->spaceship($left, $right));
        $this->assertEquals(-$expected, $this->spaceship($right, $left));
    }

    public function comparisonTable()
    {
        return array(
            array(new Duration(0, 9, 9), new Duration(9, 0, 0), -1),
            array(new Duration(9, 0, 9), new Duration(9, 9, 0), -1),
            array(new Duration(9, 9, 0), new Duration(9, 9, 9), -1),
            array(new Duration(1, 2, 3), new Duration(1, 2, 3), 0)
        );
    }

    public function testCompareDifferentTypes()
    {
        $this->assertEquals(1, $this->spaceship(new Duration(1, 2, 3), new Decimal("1.2")));
    }

    public function testProperties()
    {
        $duration = new Duration(1, 2, 3);
        $props = get_object_vars($duration);
        $this->assertEquals(array("months" => 1, "days" => 2, "nanos" => 3), $props);
    }

    /**
     * Spaceship (<=>) impl for testing our compare function. The operator exists in PHP7, but not
     * PHP 5, so we implement it ourselves as a function.
    */
    public function spaceship($left, $right) {
        if ($left < $right) {
            return -1;
        } else if ($left == $right) {
            return 0;
        } else {
            return 1;
        }
    }
}
