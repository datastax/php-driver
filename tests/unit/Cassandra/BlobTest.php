<?php

namespace Cassandra;

/**
 * @requires extension cassandra
 */
class BlobTest extends \PHPUnit_Framework_TestCase
{
    public function testHexEncodesString()
    {
        $blob = new Blob("Hi");
        $this->assertEquals("0x4869", $blob->__toString());
        $this->assertEquals("0x4869", $blob->bytes());
    }

    public function testReturnsOriginalBytesAsBinaryString()
    {
        $blob = new Blob("Hi");
        $this->assertEquals("Hi", (string) $blob->toBinaryString());
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
            array(new Blob("0x1234"), new Blob("0x1234")),
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
            array(new Blob("0x1234"), new Blob("0x4567")),
        );
    }
}
