<?php

namespace Cassandra;

/**
 * @requires extension cassandra
 */
class UuidTest extends \PHPUnit_Framework_TestCase
{
    public function testGeneratesUniqueUuids()
    {
        for ($i = 0; $i < 10000; $i++) {
            $this->assertNotEquals((string) new Uuid(), (string) new Uuid());
        }
    }
}
