<?php

namespace Cassandra\Type;

use Cassandra\Type;

/**
 * @requires extension cassandra
 */
class UdtTest extends \PHPUnit_Framework_TestCase
{
    public function testDefinesUdtType()
    {
        $type = Type::udt('a', Type::varchar());
        $this->assertEquals('udt', $type->name());
        $this->assertEquals('udt<a:varchar>', (string) $type);
        $types = $type->types();
        $this->assertEquals(Type::varchar(), $types['a']);
    }

    public function testCreatesUdtFromValues()
    {
        $udt = Type::udt('a', Type::varchar(), 'b', Type::int())
                    ->create('a', 'xyz', 'b', 123);
        $this->assertEquals(array('a' => 'xyz', 'b' => 123), $udt->values());
        $this->assertEquals('xyz', $udt->get('a'));
        $this->assertEquals(123, $udt->get('b'));
    }

    public function testCreatesEmptyUdt()
    {
        $udt = Type::udt('a', Type::varchar())->create();
        $this->assertEquals(0, count($udt));
    }

    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage Not enough name/type pairs,
     *                           udts can only be created from an even
     *                           number of name/type pairs, where each
     *                           odd argument is a name and each even
     *                           argument is a type,
     *                           e.g udt(name, type, name, type, name, type)'
     *                           contains 'argument must be a string, '1' given
     */
    public function testPreventsCreatingUdtTypeWithInvalidName()
    {
        Type::udt(Type::varchar());
    }

    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage Not enough name/value pairs,
     *                           udts can only be created from an even
     *                           number of name/value pairs, where each
     *                           odd argument is a name and each even
     *                           argument is a value,
     *                           e.g udt(name, value, name, value, name, value)'
     *                           contains 'argument must be a string, '1' given'.
     */
    public function testPreventsCreatingUdtWithInvalidName()
    {
        Type::udt('a', Type::varchar())->create(1);
    }


    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage argument must be a string, '1' given
     */
    public function testPreventsCreatingUdtWithUnsupportedTypes()
    {
        Type::udt('a', Type::varchar())->create('a', 1);
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
    public function testPreventsDefiningUdtsWithUnsupportedTypes()
    {
        Type::udt('a', new UnsupportedType());
    }
}
