<?php

namespace Cassandra\Type;

use Cassandra\Type;

/**
 * @requires extension cassandra
 */
class UserTypeTest extends \PHPUnit_Framework_TestCase
{
    public function testDefinesUserTypeType()
    {
        $type = Type::userType('a', Type::varchar());
        $this->assertEquals('userType<a:varchar>', (string) $type);
        $types = $type->types();
        $this->assertEquals(Type::varchar(), $types['a']);
    }

    public function testCreatesUserTypeFromValues()
    {
        $udt = Type::userType('a', Type::varchar(), 'b', Type::int())
                    ->create('a', 'xyz', 'b', 123);
        $this->assertEquals(array('a' => 'xyz', 'b' => 123), $udt->values());
        $this->assertEquals('xyz', $udt->get('a'));
        $this->assertEquals(123, $udt->get('b'));
    }

    public function testCreatesEmptyUserType()
    {
        $udt = Type::userType('a', Type::varchar())->create();
        $this->assertEquals(1, count($udt));
        $this->assertEquals($udt->get('a'), null);

        $udt = Type::userType('a', Type::varchar(), 'b', Type::int())->create();
        $this->assertEquals(2, count($udt));
        $this->assertEquals($udt->get('a'), null);
        $this->assertEquals($udt->get('b'), null);

        $udt = Type::userType('a', Type::varchar(), 'b', Type::int(), 'c', Type::bigint())->create();
        $this->assertEquals(3, count($udt));
        $this->assertEquals($udt->get('a'), null);
        $this->assertEquals($udt->get('b'), null);
        $this->assertEquals($udt->get('c'), null);
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
    public function testPreventsCreatingUserTypeTypeWithInvalidName()
    {
        Type::userType(Type::varchar());
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
    public function testPreventsCreatingUserTypeWithInvalidName()
    {
        Type::userType('a', Type::varchar())->create(1);
    }


    /**
     * @expectedException        InvalidArgumentException
     * @expectedExceptionMessage argument must be a string, '1' given
     */
    public function testPreventsCreatingUserTypeWithUnsupportedTypes()
    {
        Type::userType('a', Type::varchar())->create('a', 1);
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
    public function testPreventsDefiningUserTypesWithUnsupportedTypes()
    {
        Type::userType('a', new UnsupportedType());
    }

    /**
     * @dataProvider equalTypes
     */
    public function testCompareEquals($type1, $type2)
    {
        $this->assertEquals($type1, $type2);
        $this->assertTrue($type1 == $type2);
    }

    public function equalTypes()
    {
        return array(
            array(Type::userType('a', Type::int()),
                  Type::userType('a', Type::int())),
            array(Type::userType('a', Type::int(), 'b', Type::varchar()),
                  Type::userType('a', Type::int(), 'b', Type::varchar())),
            array(Type::userType('a', Type::int(), 'b', Type::varchar(), 'c', Type::bigint()),
                  Type::userType('a', Type::int(), 'b', Type::varchar(), 'c', Type::bigint())),
            array(Type::userType('a', Type::collection(Type::int()), 'b', Type::set(Type::int())),
                  Type::userType('a', Type::collection(Type::int()), 'b', Type::set(Type::int())))
        );
    }

    /**
     * @dataProvider notEqualTypes
     */
    public function testCompareNotEquals($type1, $type2)
    {
        $this->assertNotEquals($type1, $type2);
        $this->assertFalse($type1 == $type2);
    }

    public function notEqualTypes()
    {
        return array(
            // Different types
            array(Type::userType('a', Type::int()),
                  Type::userType('a', Type::varchar())),
            array(Type::userType('a', Type::int(), 'b', Type::varchar()),
                  Type::userType('a', Type::int(), 'b', Type::bigint())),
            array(Type::userType('a', Type::int(), 'b', Type::varchar(), 'c', Type::varint()),
                  Type::userType('a', Type::int(), 'b', Type::varchar(), 'c', Type::bigint())),
            array(Type::userType('a', Type::collection(Type::int()), 'b', Type::set(Type::varchar())),
                  Type::userType('a', Type::collection(Type::int()), 'b', Type::set(Type::int()))),
            // Different names
            array(Type::userType('a', Type::int()),
                  Type::userType('b', Type::int())),
            array(Type::userType('a', Type::int(), 'c', Type::varchar()),
                  Type::userType('b', Type::int(), 'c', Type::varchar())),
        );
    }
}
