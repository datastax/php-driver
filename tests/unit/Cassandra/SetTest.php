<?php

/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

namespace Cassandra;

use Cassandra;
use InvalidArgumentException;
use PHPUnit\Framework\TestCase;
use stdClass;

/**
 * @requires extension cassandra
 */
class SetTest extends TestCase
{
    public function testInvalidType()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage(
            "type must be a string or an instance of Cassandra\Type, an instance of stdClass given"
        );
        new Set(new stdClass());
    }

    public function testUnsupportedStringType()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Unsupported type 'custom type'");
        new Set('custom type');
    }

    public function testUnsupportedType()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage(
            "type must be a valid Cassandra\Type, an instance of Cassandra\Type\UnsupportedType given"
        );
        new Set(new Type\UnsupportedType());
    }

    public function testContainsUniqueValues()
    {
        $set = new Set(Cassandra::TYPE_VARINT);
        $this->assertEquals(0, count($set));
        $set->add(new Varint('123'));
        $this->assertEquals(1, count($set));
        $set->add(new Varint('123'));
        $this->assertEquals(1, count($set));
    }

    /**
     * @dataProvider scalarTypes
     */
    public function testScalarKeys($type, $values)
    {
        $set = Type::set($type)->create();
        foreach($values as $value) {
            $set->add($value);
        }
        $this->assertEquals(count($values), count($set));

        foreach($values as $value) {
            $this->assertTrue($set->has($value));
        }

        foreach($values as $value) {
            $set->remove($value);
        }
        $this->assertEquals(0, count($set));
    }

    public function scalarTypes()
    {
        return array(
            array(Type::ascii(), array("ascii")),
            array(Type::bigint(), array(new Bigint("9223372036854775807"))),
            array(Type::blob(), array(new Blob("blob"))),
            array(Type::boolean(), array(true, false)),
            array(Type::counter(), array(new Bigint(123))),
            array(Type::decimal(), array(new Decimal("3.14159265359"))),
            array(Type::double(), array(3.14159)),
            array(Type::float(), array(new Float(3.14159))),
            array(Type::inet(), array(new Inet("127.0.0.1"))),
            array(Type::int(), array(123)),
            array(Type::text(), array("text")),
            array(Type::timestamp(), array(new Timestamp(123))),
            array(Type::timeuuid(), array(new Timeuuid(0))),
            array(Type::uuid(), array(new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2"))),
            array(Type::varchar(), array("varchar")),
            array(Type::varint(), array(new Varint("9223372036854775808"))),
            array(Type::duration(), array(new Duration(1, 2, 3)))
        );
    }

    /**
     * @dataProvider compositeTypes
     */
    public function testCompositeKeys($type)
    {
        $map = Type::set($type)->create();

        $map->add($type->create("a", "1", "b", "2"));
        $this->assertTrue($map->has($type->create("a", "1", "b", "2")));
        $this->assertEquals(1, count($map));

        $map->add($type->create("c", "3", "d", "4", "e", "5"));
        $this->assertTrue($map->has($type->create("c", "3", "d", "4", "e", "5")));
        $this->assertEquals(2, count($map));

        $map->remove($type->create("a", "1", "b", "2"));
        $this->assertFalse($map->has($type->create("a", "1", "b", "2")));
        $this->assertEquals(1, count($map));

        $map->remove($type->create("c", "3", "d", "4", "e", "5"));
        $this->assertFalse($map->has($type->create("c", "3", "d", "4", "e", "5")));
        $this->assertEquals(0, count($map));
    }

    public function compositeTypes()
    {
        return array(
            array(Type::map(Type::varchar(), Type::varchar())),
            array(Type::set(Type::varchar())),
            array(Type::collection(Type::varchar()))
        );
    }

    public function testSupportsOnlyCassandraTypes()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Unsupported type 'some custom type'");
        new Set('some custom type');
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testSupportsAllCassandraTypes($type)
    {
        $var = new Set($type);
        $this->assertEquals("set<$type>", (string)$var->type());
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
            array(Cassandra::TYPE_ASCII),
            array(Cassandra::TYPE_BIGINT),
            array(Cassandra::TYPE_BLOB),
            array(Cassandra::TYPE_BOOLEAN),
            array(Cassandra::TYPE_COUNTER),
            array(Cassandra::TYPE_DECIMAL),
            array(Cassandra::TYPE_DOUBLE),
            array(Cassandra::TYPE_FLOAT),
            array(Cassandra::TYPE_INT),
            array(Cassandra::TYPE_TEXT),
            array(Cassandra::TYPE_TIMESTAMP),
            array(Cassandra::TYPE_UUID),
            array(Cassandra::TYPE_VARCHAR),
            array(Cassandra::TYPE_VARINT),
            array(Cassandra::TYPE_TIMEUUID),
            array(Cassandra::TYPE_INET),
        );
    }

    public function testValidatesTypesOfElements()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage(
            "argument must be an instance of Cassandra\Varint, an instance of Cassandra\Decimal given"
        );
        $set = new Set(Cassandra::TYPE_VARINT);
        $set->add(new Decimal('123'));
    }

    public function testSupportsNullValues()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid value: null is not supported inside sets");
        $set = new Set(Cassandra::TYPE_VARINT);
        $set->add(null);
    }

    /**
     * @dataProvider sampleNumbers
     */
    public function testSupportsIteration($numbers)
    {
        $set = new Set(Cassandra::TYPE_INT);

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
        $set = new Set(Cassandra::TYPE_INT);

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
        $set = new Set(Cassandra::TYPE_INT);

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

    public function testSupportsRetrievingValues()
    {
        $values = array(new Varint('1'), new Varint('2'), new Varint('3'),
                        new Varint('4'), new Varint('5'), new Varint('6'),
                        new Varint('7'), new Varint('8'));
        $set = new Set(Cassandra::TYPE_VARINT);

        for ($i = 0; $i < count($values); $i++) {
            $set->add($values[$i]);
        }

        $this->assertEquals($values, $set->values());

    }

    /**
     * @dataProvider equalTypes
     */
    public function testCompareEquals($value1, $value2)
    {
        $this->assertEquals($value1, $value2);
        $this->assertTrue($value1 == $value2);
    }

    public function equalTypes()
    {
        $setType = Type::set(Type::int());
        return array(
            array(Type::set(Type::int())->create(),
                  Type::set(Type::int())->create()),
            array(Type::set(Type::int())->create(1, 2, 3),
                  Type::set(Type::int())->create(1, 2, 3)),
            array(Type::set(Type::varchar())->create('a', 'b', 'c'),
                  Type::set(Type::varchar())->create('a', 'b', 'c')),
            array(Type::set($setType)->create($setType->create(1, 2, 3)),
                  Type::set($setType)->create($setType->create(1, 2, 3))),
        );
    }

    /**
     * @dataProvider notEqualTypes
     */
    public function testCompareNotEquals($value1, $value2)
    {
        $this->assertNotEquals($value1, $value2);
        $this->assertFalse($value1 == $value2);
    }

    public function notEqualTypes()
    {
        $setType = Type::set(Type::int());
        return array(
            array(Type::set(Type::int())->create(),
                  Type::set(Type::varchar())->create()),
            array(Type::set(Type::int())->create(1, 2, 3),
                  Type::set(Type::int())->create(4, 5, 6)),
            array(Type::set(Type::int())->create(1, 2, 3),
                  Type::set(Type::int())->create(1)),
            array(Type::set(Type::varchar())->create('a', 'b', 'c'),
                  Type::set(Type::varchar())->create('a', 'b', 'd')),
            array(Type::set($setType)->create($setType->create(1, 2, 3)),
                  Type::set($setType)->create($setType->create(4, 5, 6))),
        );
    }

    public function testCompareNotEqualDifferentCount()
    {
        $this->assertTrue(Type::set(Type::int())->create(1) <
                          Type::set(Type::int())->create(1, 2));
        $this->assertTrue(Type::set(Type::int())->create(1, 2) >
                          Type::set(Type::int())->create(1));
    }
}
