<?php

namespace Cassandra;

use Cassandra\Type;

/**
 * @requires extension cassandra
 */
class MapTest extends \PHPUnit_Framework_TestCase
{
    public function testSupportsKeyBasedAccess()
    {
        $map = Type::map(Type::varint(), Type::varchar())->create();
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
     * @dataProvider scalarTypes
     */
    public function testScalarKeys($keyType, $keyValue, $keyValueCopy)
    {
        $map = Type::map($keyType, Type::varchar())->create();
        $map->set($keyValue, "value");
        $this->assertEquals(1, count($map));
        $this->assertEquals($map->get($keyValue), "value");
        $this->assertEquals($map->get($keyValueCopy), "value");
        $this->assertTrue($map->has($keyValue));
        $this->assertTrue($map->has($keyValueCopy));
        $map->remove($keyValue);
        $this->assertEquals(0, count($map));
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
        $map = Type::map($keyType, Type::varchar())->create();

        $map->set($keyType->create("a", "1", "b", "2"), "value1");
        $this->assertEquals($map->get($keyType->create("a", "1", "b", "2")), "value1");
        $this->assertEquals(1, count($map));

        $map->set($keyType->create("c", "3", "d", "4", "e", "5"), "value2");
        $this->assertEquals($map->get($keyType->create("c", "3", "d", "4", "e", "5")), "value2");
        $this->assertEquals(2, count($map));

        $map->remove($keyType->create("a", "1", "b", "2"));
        $this->assertFalse($map->has($keyType->create("a", "1", "b", "2")));
        $this->assertEquals(1, count($map));

        $map->remove($keyType->create("c", "3", "d", "4", "e", "5"));
        $this->assertFalse($map->has($keyType->create("c", "3", "d", "4", "e", "5")));
        $this->assertEquals(0, count($map));
    }

    public function compositeTypes()
    {
        return array(
            array(
                Type::map(Type::varchar(), Type::varchar()),
                Type::set(Type::varchar()),
                Type::collection(Type::varchar()))
        );
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unsupported type 'custom type'
     */
    public function testSupportsOnlyCassandraTypesForKeys()
    {
        new Map('custom type', \Cassandra::TYPE_VARINT);
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unsupported type 'another custom type'
     */
    public function testSupportsOnlyCassandraTypesForValues()
    {
        new Map(\Cassandra::TYPE_VARINT, 'another custom type');
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Invalid value: null is not supported inside maps
     */
    public function testSupportsNullValues()
    {
        $map = new Map(\Cassandra::TYPE_VARCHAR, \Cassandra::TYPE_VARCHAR);
        $map->set("test", null);
    }
}
