<?php

namespace Cassandra\Type;

use Cassandra\Type;

/**
 * @requires extension cassandra
 */
class TupleTest extends \PHPUnit_Framework_TestCase
{
    public function testDefinesTupleType()
    {
        //$type = Type::tuple(Type::varchar(), Type::int());
        //$this->assertEquals('tuple', $type->name());
        //$this->assertEquals('tuple<varchar, int>', (string) $type);
        //$types = $type->types();
        //$this->assertEquals(Type::varchar(), $types[0]);
        //$this->assertEquals(Type::int(), $types[1]);
    }

    public function testCreatesTupleFromValues()
    {
        //$tuple = Type::tuple(Type::varchar(), Type::int())
        //            ->create('xyz', 123);
        //$this->assertEquals(array('xyz', 123), $tuple->values());
        //$this->assertEquals('xyz', $tuple->get(0));
        //$this->assertEquals(123, $tuple->get(1));
    }

    public function testCreatesNullTuple()
    {
        $tuple = Type::tuple(Type::varchar())->create();
        //$this->assertEquals(1, count($tuple));
        $this->assertEquals($tuple->get(0), null);
    }

    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage argument must be a string, '1' given
     */
    public function testPreventsCreatingTupleWithInvalidType()
    {
        Type::tuple(Type::varchar())->create(1);
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
     *                           Cassandra\Type::timestamp(), Cassandra\Type::uuid(),
     *                           Cassandra\Type::timeuuid(), Cassandra\Type::map(),
     *                           Cassandra\Type::set(), Cassandra\Type::collection(),
     *                           Cassandra\Type::tuple() or Cassandra\Type::udt(),
     *                           an instance of Cassandra\Type\UnsupportedType given
     */
    public function testPreventsDefiningTuplesWithUnsupportedTypes()
    {
        Type::tuple(new UnsupportedType());
    }
}
