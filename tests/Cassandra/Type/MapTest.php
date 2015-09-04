<?php

namespace Cassandra\Type;

use Cassandra\Type;

class UnsupportedType implements Type
{
    public function name()
    {
        return "unsupported";
    }

    public function __toString()
    {
        return "unsupported";
    }

    public function create($value = null)
    {
        return null;
    }
}

/**
 * @requires extension cassandra
 */
class MapTest extends \PHPUnit_Framework_TestCase
{
    public function testDefinesMapType()
    {
        $type = Type::map(Type::varchar(), Type::int());
        $this->assertEquals("map", $type->name());
        $this->assertEquals("map<varchar, int>", (string) $type);
        $this->assertEquals(Type::varchar(), $type->keyType());
        $this->assertEquals(Type::int(), $type->valueType());
    }

    public function testCreatesMapFromValues()
    {
        $map = Type::map(Type::varchar(), Type::int())
                   ->create("a", 1, "b", 2, "c", 3, "d", 4, "e", 5);
        $this->assertEquals(array("a", "b", "c", "d", "e"), $map->keys());
        $this->assertEquals(array(1, 2, 3, 4, 5), $map->values());
        $this->assertEquals(1, $map["a"]);
        $this->assertEquals(2, $map["b"]);
        $this->assertEquals(3, $map["c"]);
        $this->assertEquals(4, $map["d"]);
        $this->assertEquals(5, $map["e"]);
    }

    public function testCreatesEmptyMap()
    {
        $map = Type::map(Type::varchar(), Type::int())->create();
        $this->assertEquals(0, count($map));
    }

    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage Not enough values, maps can only be created
     *                           from an even number of values, where each odd
     *                           value is a key and each even value is a value,
     *                           e.g create(key, value, key, value, key, value)
     */
    public function testPreventsCreatingMapWithoutEnoughValues()
    {
        Type::map(Type::varchar(), Type::int())
            ->create("a", 1, "b", 2, "c", 3, "d", 4, "e");
    }

    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage argument must be a string, '1' given
     */
    public function testPreventsCreatingMapWithUnsupportedTypes()
    {
        Type::map(Type::varchar(), Type::int())
            ->create(1, "a");
    }

    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage keyType must be Cassandra\Type::varchar(),
     *                           Cassandra\Type::text(), Cassandra\Type::blob(),
     *                           Cassandra\Type::ascii(), Cassandra\Type::bigint(),
     *                           Cassandra\Type::counter(), Cassandra\Type::int(),
     *                           Cassandra\Type::varint(), Cassandra\Type::boolean(),
     *                           Cassandra\Type::decimal(), Cassandra\Type::double(),
     *                           Cassandra\Type::float(), Cassandra\Type::inet(),
     *                           Cassandra\Type::timestamp(), Cassandra\Type::uuid()
     *                           or Cassandra\Type::timeuuid(), an instance of
     *                           Cassandra\Type\UnsupportedType given
     */
    public function testPreventsDefiningMapsWithUnsupportedTypes()
    {
        Type::map(new UnsupportedType(), Type::varchar());
    }
}
