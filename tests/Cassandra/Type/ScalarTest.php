<?php

namespace Cassandra\Type;

use Cassandra\Type;

/**
 * @requires extension cassandra
 */
class ScalarTest extends \PHPUnit_Framework_TestCase
{
    public function testAllowCreatingTypes()
    {
        $this->assertEquals("some string", Type::varchar()->create("some string"));
    }
}
