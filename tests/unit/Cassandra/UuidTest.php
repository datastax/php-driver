<?php

namespace Cassandra;

/**
 * @requires extension cassandra
 */
class UuidTest extends \PHPUnit_Framework_TestCase
{
    public function testGeneratesUniqueUuids()
    {
        for ($i = 0; $i < 10000; $i++) {
            $this->assertNotEquals((string) new Uuid(), (string) new Uuid());
        }
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
            array(new Uuid('2a5072fa-7da4-4ccd-a9b4-f017a3872304'), new Uuid('2a5072fa-7da4-4ccd-a9b4-f017a3872304')),
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
            array(new Uuid('2a5072fa-7da4-4ccd-a9b4-f017a3872304'), new Uuid('3b5072fa-7da4-4ccd-a9b4-f017a3872304')),
        );
    }
}
