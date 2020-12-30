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

use Cassandra\Exception\InvalidArgumentException;
use Datetime;
use PHPUnit\Framework\TestCase;

/**
 * @requires extension cassandra
 */
class TimeUuidTest extends TestCase
{
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
        return array(
            array(new TimeUuid(0), new TimeUuid(0))
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
        return array(
            array(new TimeUuid(0), new TimeUuid(2))
        );
    }

    /**
     * TimeUuid can be created from string
     */
    public function testInitFromStringType1()
    {
        $uuid = new TimeUuid('5f344f20-52a3-11e7-915b-5f4f349b532d');
        $this->assertIsObject($uuid);
    }

    /**
     * TimeUuid cannot be created from UUID type 4
     */
    public function testInitFromStringType4()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("UUID must be of type 1, type 4 given");
        new TimeUuid('65f9e722-036a-4029-b03b-a9046b23b4c9');
    }

    /**
     * TimeUuid cannot be created from invalid string
     */
    public function testInitFromInvalidString()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid UUID");
        new TimeUuid('invalid');
    }

    /**
     * TimeUuid requires string or integer in constructor
     */
    public function testInitInvalidArgument()
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage("Invalid argument");
        new TimeUuid(new Datetime());
    }
}
