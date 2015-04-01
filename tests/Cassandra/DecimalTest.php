<?php

namespace Cassandra;

class DecimalTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unrecognized character 'q' at 0
     */
    public function testThrowsWhenCreatingNotAnInteger()
    {
        new Decimal("qwe");
    }

    /**
     * @dataProvider validNumbers
     */
    public function testCorrectlyParsesStrings($number, $value, $scale, $string)
    {
        $number = new Decimal($number);
        $this->assertEquals($value, $number->value());
        $this->assertEquals($scale, $number->scale());
        $this->assertEquals($string, (string) $number);
    }

    public function validNumbers()
    {
        return array(
            array("123", "123", 0, "123"),
            array("0123", "83", 0, "83"),
            array("0x123", "291", 0, "291"),
            array("0b1010101", "85", 0, "85"),
            array("-123", "-123", 0, "-123"),
            array("-0123", "-83", 0, "-83"),
            array("-0x123", "-291", 0, "-291"),
            array("-0b1010101", "-85", 0, "-85"),
            array("1313123123.234234234234234234123", "1313123123234234234234234234123", 21, "1313123123.234234234234234234123")
        );
    }
}
