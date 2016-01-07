<?php

namespace Cassandra;

/**
 * @requires extension cassandra
 */
class SetTest extends \PHPUnit_Framework_TestCase
{
    public function testContainsUniqueValues()
    {
        $set = new Set(\Cassandra::TYPE_VARINT);
        $this->assertEquals(0, count($set));
        $set->add(new Varint('123'));
        $this->assertEquals(1, count($set));
        $set->add(new Varint('123'));
        $this->assertEquals(1, count($set));
    }

    /**
     * @dataProvider scalarTypes
     */
    public function testScalarKeys($keyType, $keyValue, $keyValueCopy)
    {
        $map = Type::set($keyType)->create();
        $map->add($keyValue);
        $this->assertEquals(1, count($map));
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
        $map = Type::set($keyType)->create();

        $map->add($keyType->create("a", "1", "b", "2"));
        $this->assertTrue($map->has($keyType->create("a", "1", "b", "2")));
        $this->assertEquals(1, count($map));

        $map->add($keyType->create("c", "3", "d", "4", "e", "5"));
        $this->assertTrue($map->has($keyType->create("c", "3", "d", "4", "e", "5")));
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
     * @expectedExceptionMessage  Unsupported type 'some custom type'
     */
    public function testSupportsOnlyCassandraTypes()
    {
        new Set('some custom type');
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testSupportsAllCassandraTypes($type)
    {
        new Set($type);
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testReturnsItsType($type)
    {
        $set = new Set($type);
        $this->assertEquals($type, $set->type()->valueType());
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
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  argument must be an instance of Cassandra\Varint, an instance of Cassandra\Decimal given
     */
    public function testValidatesTypesOfElements()
    {
        $set = new Set(\Cassandra::TYPE_VARINT);
        $set->add(new Decimal('123'));
    }

    /**
     * @dataProvider sampleNumbers
     */
    public function testSupportsIteration($numbers)
    {
        $set = new Set(\Cassandra::TYPE_INT);

        foreach ($numbers as $number) {
            $set->add($number);
        }

        $this->assertEquals(count($numbers), count($set));

        foreach ($set as $i => $number) {
            $this->assertEquals($numbers[$i], $number);
        }
    }

    /**
     * @dataProvider sampleNumbers
     */
    public function testSupportsConversionToArray($numbers)
    {
        $set = new Set(\Cassandra::TYPE_INT);

        foreach ($numbers as $number) {
            $set->add($number);
        }

        $this->assertEquals($numbers, $set->values());
    }

    /**
     * @dataProvider sampleNumbers
     */
    public function testResumesIterationAfterConvertingToArray($numbers)
    {
        $set = new Set(\Cassandra::TYPE_INT);

        foreach ($numbers as $number) {
            $set->add($number);
        }

        $set->rewind();

        $this->assertEquals(1, $set->current());
        $set->next();
        $this->assertEquals(2, $set->current());
        $set->next();
        $this->assertEquals(3, $set->current());

        $set->values();

        $set->next();
        $this->assertEquals(4, $set->current());
        $set->next();
        $this->assertEquals(5, $set->current());
        $set->next();
        $this->assertEquals(6, $set->current());
    }

    public function sampleNumbers()
    {
        return array(
            array(array(1, 2, 3, 4, 5, 6, 7, 8, 9))
        );
    }
}
