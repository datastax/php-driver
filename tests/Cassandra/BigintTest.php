<?php

namespace Cassandra;
namespace Cassandra\Types;

/**
 * @requires extension cassandra
 */
class BigintTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Non digit characters were found in value: '123.123'
     */
    public function testThrowsWhenCreatingNotAnInteger()
    {
        new Bigint("123.123");
    }

    /**
     * @dataProvider validStrings
     */
    public function testCorrectlyParsesStrings($number, $expected)
    {
        $number = new Bigint($number);
        $this->assertEquals($expected, $number->value());
        $this->assertEquals($expected, (string) $number);
    }

    public function validStrings()
    {
        return array(
            array("123", "123"),
            array("0123", "83"),
            array("0x123", "291"),
            array("0b1010101", "85"),
            array("-123", "-123"),
            array("-0123", "-83"),
            array("-0x123", "-291") ,
            array("-0b1010101", "-85")
        );
    }

    /**
     * @dataProvider validNumbers
     */
    public function testFromNumbers($number)
    {
        $bigint = new Bigint($number);
        $this->assertEquals((int)$number, $bigint->toInt());
        $this->assertEquals((float)(int)$number, $bigint->toDouble());
        $this->assertEquals((string)(int)$number, (string)$bigint);
    }

    public function validNumbers()
    {
        return array(
            array(0.123),
            array(123),
        );
    }

    public function testIs32Bit()
    {
        if (PHP_INT_MAX == 9223372036854775807) {
            $this->markTestSkipped("Not a valid test on 64-bit machinces");
        }
    }

    /**
     * @depends testIs32Bit
     * @expectedException         RangeException
     * @expectedExceptionMessage  Value is too big
     */
    public function testOverflowTooBig()
    {
        $bigint = new Bigint("9223372036854775807");
        $i = $bigint->toInt();
    }

    /**
     * @depends testIs32Bit
     * @expectedException         RangeException
     * @expectedExceptionMessage  Value is too small
     */
    public function testOverflowTooSmall()
    {
        $bigint = new Bigint("-9223372036854775808");
        $i = $bigint->toInt();
    }
}
