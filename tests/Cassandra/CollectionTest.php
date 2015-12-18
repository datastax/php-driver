<?php

namespace Cassandra;

/**
 * @requires extension cassandra
 */
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
        $this->assertEquals($type, $list->valueType());
    }

    public function cassandraTypes()
    {
        return array(
            array(\Cassandra::TYPE_ASCII),
            array(\Cassandra::TYPE_BIGINT),
            array(\Cassandra::TYPE_BLOB),
            array(\Cassandra::TYPE_BOOLEAN),
            array(\Cassandra::TYPE_COUNTER),
            array(\Cassandra::TYPE_DECIMAL),
            array(\Cassandra::TYPE_DOUBLE),
            array(\Cassandra::TYPE_FLOAT),
            array(\Cassandra::TYPE_INT),
            array(\Cassandra::TYPE_TEXT),
            array(\Cassandra::TYPE_TIMESTAMP),
            array(\Cassandra::TYPE_UUID),
            array(\Cassandra::TYPE_VARCHAR),
            array(\Cassandra::TYPE_VARINT),
            array(\Cassandra::TYPE_TIMEUUID),
            array(\Cassandra::TYPE_INET),
        );
    }

    /**
     * @dataProvider scalarTypes
     */
    public function testScalarKeys($keyType, $keyValue, $keyValueCopy)
    {
        $map = Type::collection($keyType)->create();
        $map->add($keyValue);
        $this->assertEquals(1, count($map));
        $this->assertEquals($map->find($keyValue), 0);
        $this->assertEquals($map->find($keyValueCopy), 0);
    }

    public function scalarTypes()
    {
        return array(
            array(Type::ascii(), "ascii", "ascii"),
            array(Type::bigint(), new Bigint("9223372036854775807"), new Bigint("9223372036854775807")),
            array(Type::blob(), new Blob("blob"), new Blob("blob")),
            array(Type::boolean(), true, true),
            array(Type::counter(), new Bigint(123), new Bigint(123)),
            array(Type::decimal(), new Decimal("3.14159265359"), new Decimal("3.14159265359")),
            array(Type::double(), 3.14159, 3.14159),
            array(Type::float(), new Float(3.14159), new Float(3.14159)),
            array(Type::inet(), new Inet("127.0.0.1"), new Inet("127.0.0.1")),
            array(Type::int(), 123, 123),
            array(Type::text(), "text", "text"),
            array(Type::timestamp(), new Timestamp(123), new Timestamp(123)),
            array(Type::timeuuid(), new Timeuuid(0), new Timeuuid(0)),
            array(Type::uuid(), new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2"), new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2")),
            array(Type::varchar(), "varchar", "varchar"),
            array(Type::varint(), new Varint("9223372036854775808"), new Varint("9223372036854775808"))
        );
    }

    /**
     * @dataProvider compositeTypes
     */
    public function testCompositeKeys($keyType)
    {
        $map = Type::collection($keyType)->create();

        $map->add($keyType->create("a", "1", "b", "2"));
        $this->assertEquals($map->find($keyType->create("a", "1", "b", "2")), 0);
        $this->assertEquals(1, count($map));

        $map->add($keyType->create("c", "3", "d", "4", "e", "5"));
        $this->assertEquals($map->find($keyType->create("c", "3", "d", "4", "e", "5")), 1);
        $this->assertEquals(2, count($map));

        $map->remove(0);
        $this->assertNull($map->find($keyType->create("a", "1", "b", "2")));
        $this->assertEquals(1, count($map));

        $map->remove(1);
        $this->assertNull($map->find($keyType->create("c", "3", "d", "4", "e", "5")));
        $this->assertEquals(0, count($map));
    }

    public function compositeTypes()
    {
        return array(
            array(
                Type::map(Type::varchar(), Type::varchar()),
                #Type::set(Type::varchar()),
                #Type::collection(Type::varchar())
            )
        );
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  argument must be an instance of Cassandra\Varint, an instance of Cassandra\Decimal given
     */
    public function testValidatesTypesOfElements()
    {
        $list = new Collection(\Cassandra::TYPE_VARINT);
        $list->add(new Decimal('123'));
    }

    public function testAddsAllElements()
    {
        $list = new Collection(\Cassandra::TYPE_VARINT);
        $list->add(new Varint('1'), new Varint('2'), new Varint('3'),
                   new Varint('4'), new Varint('5'), new Varint('6'),
                   new Varint('7'), new Varint('8'));

        $this->assertEquals(8, $list->count());
        $this->assertEquals(
            array(
                new Varint('1'), new Varint('2'), new Varint('3'),
                new Varint('4'), new Varint('5'), new Varint('6'),
                new Varint('7'), new Varint('8')
            ),
            $list->values()
        );
    }

    public function testReturnsNullWhenCannotFindIndex()
    {
        $list = new Collection(\Cassandra::TYPE_VARINT);
        $this->assertSame(null, $list->find(new Varint('1')));
    }

    public function testFindsIndexOfAnElement()
    {
        $list = new Collection(\Cassandra::TYPE_VARINT);
        $list->add(new Varint('1'), new Varint('2'), new Varint('3'),
                   new Varint('4'), new Varint('5'), new Varint('6'),
                   new Varint('7'), new Varint('8'));

        $this->assertEquals(0, $list->find(new Varint('1')));
        $this->assertEquals(1, $list->find(new Varint('2')));
        $this->assertEquals(2, $list->find(new Varint('3')));
        $this->assertEquals(3, $list->find(new Varint('4')));
        $this->assertEquals(4, $list->find(new Varint('5')));
        $this->assertEquals(5, $list->find(new Varint('6')));
        $this->assertEquals(6, $list->find(new Varint('7')));
        $this->assertEquals(7, $list->find(new Varint('8')));
    }

    public function testGetsElementByIndex()
    {
        $list = new Collection(\Cassandra::TYPE_VARINT);
        $list->add(new Varint('1'), new Varint('2'), new Varint('3'),
                   new Varint('4'), new Varint('5'), new Varint('6'),
                   new Varint('7'), new Varint('8'));

        $this->assertEquals(new Varint('1'), $list->get(0));
        $this->assertEquals(new Varint('2'), $list->get(1));
        $this->assertEquals(new Varint('3'), $list->get(2));
        $this->assertEquals(new Varint('4'), $list->get(3));
        $this->assertEquals(new Varint('5'), $list->get(4));
        $this->assertEquals(new Varint('6'), $list->get(5));
        $this->assertEquals(new Varint('7'), $list->get(6));
        $this->assertEquals(new Varint('8'), $list->get(7));
    }
}
