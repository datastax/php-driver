<?php

namespace Cassandra\Type;

use Cassandra\Type;

/**
 * @requires extension cassandra
 */
class SetTest extends \PHPUnit_Framework_TestCase
{
    public function testDefinesSetType()
    {
        $type = Type::set(Type::varchar());
        $this->assertEquals("set", $type->name());
        $this->assertEquals("set<varchar>", (string) $type);
        $this->assertEquals(Type::varchar(), $type->valueType());
    }

    public function testCreatesSetFromValues()
    {
        $set = Type::set(Type::varchar())
                   ->create("a", "b", "c", "d", "e");
        $this->assertEquals(array("a", "b", "c", "d", "e"), $set->values());
    }

    public function testCreatesEmptySet()
    {
        $set = Type::set(Type::varchar())->create();
        $this->assertEquals(0, count($set));
    }

    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage argument must be a string, '1' given
     */
    public function testPreventsCreatingSetWithUnsupportedTypes()
    {
        Type::set(Type::varchar())->create(1);
    }

    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage type must be Cassandra\Type::varchar(),
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
    public function testPreventsDefiningSetsWithUnsupportedTypes()
    {
        Type::set(new UnsupportedType());
    }
}
