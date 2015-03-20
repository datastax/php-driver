<?php

namespace Cassandra;

class CollectionTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unsupported type 'custom type'
     */
    public function testSupportsOnlyCassandraTypes()
    {
        new Collection('custom type');
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testSupportsAllCassandraTypes($type)
    {
        new Collection($type);
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testReturnsItsType($type)
    {
        $list = new Collection($type);
        $this->assertEquals($type, $list->type());
    }

    public function cassandraTypes()
    {
        return array(
            array("ascii"),
            array("bigint"),
            array("blob"),
            array("boolean"),
            array("counter"),
            array("decimal"),
            array("double"),
            array("float"),
            array("int"),
            array("text"),
            array("timestamp"),
            array("uuid"),
            array("varchar"),
            array("varint"),
            array("timeuuid"),
            array("inet"),
        );
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Expected an instance of Cassandra\Varint, an instance of Cassandra\Decimal given
     */
    public function testValidatesTypesOfElements()
    {
        $list = new Collection('varint');
        $list->add(new Decimal('123'));
    }

    public function testAddsAllElements()
    {
        $list = new Collection('int');
        $list->add(1, 2, 3, 4, 5, 6, 7, 8);
        $this->assertEquals(8, $list->count());
        $this->assertEquals(array(1, 2, 3, 4, 5, 6, 7, 8), $list->values());
    }
}
