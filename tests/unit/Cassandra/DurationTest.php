<?php

/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 */

namespace Cassandra;

/**
 * @requires extension cassandra
 */
class DurationTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage months must be a long, a double, a numeric string or a Cassandra\Bigint, 1 given
     */
    public function testMonthsArgWrongType()
    {
        new Duration(true, 2, 3);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage days must be a long, a double, a numeric string or a Cassandra\Bigint, 1 given
     */
    public function testDaysArgWrongType()
    {
        new Duration(1, true, 3);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage nanos must be a long, a double, a numeric string or a Cassandra\Bigint, 1 given
     */
    public function testNanosArgWrongType()
    {
        new Duration(1, 2, true);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage Invalid integer value: 'ab'
     */
    public function testStringArgParseError()
    {
        new Duration(1, 2, "ab");
    }

    /**
     * @expectedException RangeException
     * @expectedExceptionMessage value must be between -9223372036854775808 and 9223372036854775807, 9223372036854775808 given
     */
    public function testString64BitArgOverflowError()
    {
        new Duration(1, 2, "9223372036854775808");
    }

    /**
     * @expectedException RangeException
     * @expectedExceptionMessage value must be between -9223372036854775808 and 9223372036854775807, -9223372036854775809 given
     */
    public function testString64BitArgUnderflowError()
    {
        new Duration(1, 2, "-9223372036854775809");
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage nanos must be between -9223372036854775808 and 9223372036854775807, 1.84467e+19 given
     */
    public function testDouble64BitArgOverflowError()
    {
        new Duration(1, 2, pow(2, 64));
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage nanos must be between -9223372036854775808 and 9223372036854775807, -1.84467e+19 given
     */
    public function testDouble64BitArgUnderflowError()
    {
        new Duration(1, 2, -pow(2, 64));
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage days must be between -2147483648 and 2147483647, 2147483648 given
     */
    public function testString32BitArgOverflowError()
    {
        new Duration(1, "2147483648", 0);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage days must be between -2147483648 and 2147483647, -2147483649 given
     */
    public function testString32BitArgUnderflowError()
    {
        new Duration(1, "-2147483649", 0);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessageRegExp /days must be between -2147483648 and 2147483647, 8\.?58993.* given/
     */
    public function testLong32BitArgOverflowError()
    {
        new Duration(1, 8589934592, 2);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessageRegExp /days must be between -2147483648 and 2147483647, -8\.?58993.* given/
     */
    public function testLong32BitArgUnderflowError()
    {
        new Duration(1, -8589934592, 2);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage months must be between -2147483648 and 2147483647, 8.58993e+9 given
     */
    public function testDouble32BitArgOverflowError()
    {
        new Duration(8589934592.5, 1, 2);
    }

    /**
     * @expectedException InvalidArgumentException
     * @expectedExceptionMessage months must be between -2147483648 and 2147483647, -8.58993e+9 given
     */
    public function testDouble32BitArgUnderflowError()
    {
        new Duration(-8589934592.5, 1, 2);
    }

    /**
     * @expectedException BadFunctionCallException
     * @expectedExceptionMessage A duration must have all non-negative or non-positive attributes
     * @dataProvider mixedSigns
     */
    public function testMixedSignError($months, $days, $nanos)
    {
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
