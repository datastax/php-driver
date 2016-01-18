<?php

namespace Cassandra;

/**
 * @requires extension cassandra
 */
class TupleTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unsupported type 'custom type'
     */
    public function testSupportsOnlyCassandraTypes()
    {
        new Tuple(array('custom type'));
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testSupportsAllCassandraTypes($types)
    {
        new Tuple($types);
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testReturnsItsType($types)
    {
        $tuple = new Tuple($types);
        $this->assertEquals(count($types), count($tuple->type()->types()));
        $this->assertEquals($types, $tuple->type()->types());
    }

    public function cassandraTypes()
    {
        return array(
            array(array(\Cassandra::TYPE_TEXT)),
            array(array(\Cassandra::TYPE_ASCII)),
            array(array(\Cassandra::TYPE_VARCHAR)),
            array(array(\Cassandra::TYPE_BIGINT)),
            array(array(\Cassandra::TYPE_BOOLEAN)),
            array(array(\Cassandra::TYPE_COUNTER)),
            array(array(\Cassandra::TYPE_DECIMAL)),
            array(array(\Cassandra::TYPE_DOUBLE)),
            array(array(\Cassandra::TYPE_FLOAT)),
            array(array(\Cassandra::TYPE_INT)),
            array(array(\Cassandra::TYPE_TIMESTAMP)),
            array(array(\Cassandra::TYPE_UUID)),
            array(array(\Cassandra::TYPE_VARINT)),
            array(array(\Cassandra::TYPE_TIMEUUID)),
            array(array(\Cassandra::TYPE_INET)),
            array(array(\Cassandra::TYPE_TIMEUUID, \Cassandra::TYPE_UUID)),
            array(array(\Cassandra::TYPE_INT, \Cassandra::TYPE_BIGINT, \Cassandra::TYPE_VARINT)),
            array(array(\Cassandra::TYPE_INT, \Cassandra::TYPE_BIGINT, \Cassandra::TYPE_VARINT)),
            array(array(\Cassandra::TYPE_ASCII, \Cassandra::TYPE_TEXT, \Cassandra::TYPE_VARCHAR)),
        );
    }

    /**
     * @dataProvider compositeTypes
     */
    public function testCompositeKeys($type, $value)
    {
        $tuple = Type::tuple($type)->create($value);
        $this->assertEquals($tuple->get(0), $value);
    }

    public function compositeTypes()
    {
        $map_type = Type::map(Type::varchar(), Type::varchar());
        $set_type = Type::set(Type::varchar());
        $list_type = Type::collection(Type::varchar());
        $tuple_type = Type::tuple(Type::varchar(), Type::int());
        return array(
            array($map_type, $map_type->create("a", "1", "b", "2")),
            array($set_type, $set_type->create("a", "b", "c")),
            array($list_type, $list_type->create("a", "b", "c")),
            array($tuple_type, $tuple_type->create("a", 42)),
        );
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  argument must be an instance of Cassandra\Varint, an instance of Cassandra\Decimal given
     */
    public function testValidatesTypesOfElements()
    {
        $tuple = new Tuple(array(\Cassandra::TYPE_VARINT));
        $tuple->set(0, new Decimal('123'));
    }

    public function testSetAllElements()
    {
        $tuple = new Tuple(array(\Cassandra::TYPE_BOOLEAN,
            \Cassandra::TYPE_INT,
            \Cassandra::TYPE_BIGINT,
            \Cassandra::TYPE_TEXT,
        ));

        $this->assertEquals(4, $tuple->count());

        $tuple->set(0, true);
        $tuple->set(1, 42);
        $tuple->set(2, new Bigint("123"));
        $tuple->set(3, "abc");

        $this->assertEquals(4, $tuple->count());
        $this->assertEquals($tuple->get(0), true);
        $this->assertEquals($tuple->get(1), 42);
        $this->assertEquals($tuple->get(2), new Bigint("123"));
        $this->assertEquals($tuple->get(3), "abc");
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Index out of bounds
     */
    public function testInvalidSetIndex()
    {
        $tuple = new Tuple(array(\Cassandra::TYPE_TEXT));
        $tuple->set(1, "invalid index");
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Index out of bounds
     */
    public function testInvalidGetIndex()
    {
        $tuple = new Tuple(array(\Cassandra::TYPE_TEXT));
        $tuple->set(0, "invalid index");
        $tuple->get(1);
    }
}