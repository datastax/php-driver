<?php

namespace Cassandra;

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
     * @dataProvider validNumbers
     */
    public function testCorrectlyParsesStrings($number, $expected)
    {
        $number = new Bigint($number);
        $this->assertEquals($expected, $number->value());
        $this->assertEquals($expected, (string) $number);
    }

    public function validNumbers()
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
}
