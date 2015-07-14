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
}
