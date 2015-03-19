<?php

namespace Cassandra;

class MapTest extends \PHPUnit_Framework_TestCase
{
    public function testSupportsKeyBasedAccess()
    {
        $map = new Map('varint', 'varchar');
        $this->assertEquals(0, count($map));
        $map->set(new Varint('123'), 'value');
        $this->assertEquals(1, count($map));
        $this->assertTrue($map->has(new Varint('123')));
        $this->assertEquals('value', $map->get(new Varint('123')));
        $map->set(new Varint('123'), 'another value');
        $this->assertEquals(1, count($map));
        $this->assertEquals('another value', $map->get(new Varint('123')));
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unsupported type 'custom type'
     */
    public function testSupportsOnlyCassandraTypesForKeys()
    {
        new Map('custom type', 'varint');
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unsupported type 'another custom type'
     */
    public function testSupportsOnlyCassandraTypesForValues()
    {
        new Map('varint', 'another custom type');
    }
}
