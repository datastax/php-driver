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

/**
 * @requires extension cassandra
 */
class TupleTest extends TestCase
{
    public function testSupportsOnlyCassandraTypes()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Unsupported type 'custom type'");
        new Tuple(array('custom type'));
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testSupportsAllCassandraTypes($types)
    {
        $tuple = new Tuple($types);
        $this->assertInstanceOf(Tuple::class, $tuple);
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
            array(array(Cassandra::TYPE_TEXT)),
            array(array(Cassandra::TYPE_ASCII)),
            array(array(Cassandra::TYPE_VARCHAR)),
            array(array(Cassandra::TYPE_BIGINT)),
            array(array(Cassandra::TYPE_BOOLEAN)),
            array(array(Cassandra::TYPE_COUNTER)),
            array(array(Cassandra::TYPE_DECIMAL)),
            array(array(Cassandra::TYPE_DOUBLE)),
            array(array(Cassandra::TYPE_FLOAT)),
            array(array(Cassandra::TYPE_INT)),
            array(array(Cassandra::TYPE_TIMESTAMP)),
            array(array(Cassandra::TYPE_UUID)),
            array(array(Cassandra::TYPE_VARINT)),
            array(array(Cassandra::TYPE_TIMEUUID)),
            array(array(Cassandra::TYPE_INET)),
            array(array(Cassandra::TYPE_TIMEUUID, Cassandra::TYPE_UUID)),
            array(array(Cassandra::TYPE_INT, Cassandra::TYPE_BIGINT, Cassandra::TYPE_VARINT)),
            array(array(Cassandra::TYPE_INT, Cassandra::TYPE_BIGINT, Cassandra::TYPE_VARINT)),
            array(array(Cassandra::TYPE_ASCII, Cassandra::TYPE_TEXT, Cassandra::TYPE_VARCHAR)),
        );
    }

    /**
     * @dataProvider scalarTypes
     */
    public function testScalarTypes($type, $value)
    {
        $tuple = Type::tuple($type)->create();
        $tuple->set(0, $value);
        $this->assertEquals(1, count($tuple));
        $this->assertEquals($tuple->get(0), $value);
    }

    public function scalarTypes()
    {
        return array(
            array(Type::ascii(), "ascii"),
            array(Type::bigint(), new Bigint("9223372036854775807")),
            array(Type::blob(), new Blob("blob")),
            array(Type::boolean(), true),
            array(Type::counter(), new Bigint(123)),
            array(Type::decimal(), new Decimal("3.14159265359")),
            array(Type::double(), 3.14159),
            array(Type::float(), new Float(3.14159)),
            array(Type::inet(), new Inet("127.0.0.1")),
            array(Type::int(), 123),
            array(Type::text(), "text"),
            array(Type::timestamp(), new Timestamp(123)),
            array(Type::timeuuid(), new Timeuuid(0)),
            array(Type::uuid(), new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2")),
            array(Type::varchar(), "varchar"),
            array(Type::varint(), new Varint("9223372036854775808")),
            array(Type::duration(), new Duration(1, 2, 3), new Duration(1, 2, 3))
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

    public function testValidatesTypesOfElements()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage(
            "argument must be an instance of Cassandra\Varint, an instance of Cassandra\Decimal given"
        );
        $tuple = new Tuple(array(Cassandra::TYPE_VARINT));
        $tuple->set(0, new Decimal('123'));
    }

    public function testSetAllElements()
    {
        $tuple = new Tuple(
            array(
                Cassandra::TYPE_BOOLEAN,
                Cassandra::TYPE_INT,
                Cassandra::TYPE_BIGINT,
                Cassandra::TYPE_TEXT,
            )
        );

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

    public function testInvalidSetIndex()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Index out of bounds");
        $tuple = new Tuple(array(Cassandra::TYPE_TEXT));
        $tuple->set(1, "invalid index");
    }

    public function testInvalidGetIndex()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Index out of bounds");
        $tuple = new Tuple(array(Cassandra::TYPE_TEXT));
        $tuple->set(0, "invalid index");
        $tuple->get(1);
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
            array(
                Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(),
                Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create()
            ),
            array(
                Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(1, 'a', new Bigint(99)),
                Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(1, 'a', new Bigint(99))
            ),
            array(
                Type::tuple($setType, Type::varchar())->create($setType->create(1, 2, 3), 'a'),
                Type::tuple($setType, Type::varchar())->create($setType->create(1, 2, 3), 'a')
            )
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
            array(
                Type::tuple(Type::int(), Type::varchar(), Type::varint())->create(),
                Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create()
            ),
            array(
                Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(1, 'a', new Bigint(99)),
                Type::tuple(Type::int(), Type::varchar(), Type::bigint())->create(2, 'b', new Bigint(99))
            ),
            array(
                Type::tuple($setType, Type::varchar())->create($setType->create(1, 2, 3), 'a'),
                Type::tuple($setType, Type::varchar())->create($setType->create(4, 5, 6), 'a')
            )
        );
    }
}
