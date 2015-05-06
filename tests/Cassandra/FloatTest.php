<?php

namespace Cassandra;

/**
 * @requires extension cassandra
 */
class FloatTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @dataProvider      invalidStrings
     * @expectedException InvalidArgumentException
     */
    public function testThrowsWhenCreatingNotAFloat($string)
    {
        new Float($string);
    }

    public function invalidStrings()
    {
        return array(
            array(""),
            array("invalid"),
            array("123.123       "),
        );
    }

    /**
     * @dataProvider      outOfRangeStrings
     * @expectedException RangeException
     */
    public function testThrowsWhenCreatingOutOfRangeFloat($string)
    {
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
        $this->assertEquals((float)$number, (float)$expected, "", 0.00001);
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
        $this->assertEquals((float)$float, (float)$number, "", 0.00001);
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
        $this->assertEquals(1.5, (float)$float1->add($float2), "", 0.00001);
    }

    public function testSub()
    {
        $float1 = new Float("1");
        $float2 = new Float("0.5");
        $this->assertEquals(0.5, (float)$float1->sub($float2), "", 0.00001);
    }

    public function testMul()
    {
        $float1 = new Float("2");
        $float2 = new Float("0.5");
        $this->assertEquals(1.0, (float)$float1->mul($float2), "", 0.00001);
    }

    public function testDiv()
    {
        $float1 = new Float("1");
        $float2 = new Float("0.5");
        $this->assertEquals(2, (float)$float1->div($float2), "", 0.00001);
    }

    /**
     * @expectedException Cassandra\Exception\DivideByZeroException
     */
    public function testDivByZero()
    {
        $float1 = new Float("1");
        $float2 = new Float("0");
        $float1->div($float2);
    }

    /**
     * @expectedException        RuntimeException
     * @expectedExceptionMessage Not implemented
     */
    public function testMod()
    {
        $float1 = new Float("1");
        $float2 = new Float("2");
        $float1->mod($float2);
    }

    public function testAbs()
    {
        $float1 = new Float("-123.123");
        $this->assertEquals(123.123, (float)$float1->abs(), "", 0.00001);
    }

    public function testNeg()
    {
        $float1 = new Float("123.123");
        $this->assertEquals(-123.123, (float)$float1->neg(), "", 0.00001);
    }

    public function testSqrt()
    {
        $float1 = new Float("4.0");
        $this->assertEquals(2.0, (float)$float1->sqrt(), "", 0.00001);
    }
}
