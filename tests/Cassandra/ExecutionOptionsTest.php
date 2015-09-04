<?php

namespace Cassandra;

/**
 * @requires extension cassandra
 */
class ExecutionOptionsTest extends \PHPUnit_Framework_TestCase
{
    public function testAllowsRetrievingSettingsByName()
    {
        $options = new ExecutionOptions(array(
            'consistency'        => \Cassandra::CONSISTENCY_ANY,
            'serial_consistency' => \Cassandra::CONSISTENCY_LOCAL_SERIAL,
            'page_size'          => 15000,
            'timeout'            => 15,
            'arguments'          => array('a', 1, 'b', 2, 'c', 3)
        ));

        $this->assertEquals(\Cassandra::CONSISTENCY_ANY, $options->consistency);
        $this->assertEquals(\Cassandra::CONSISTENCY_LOCAL_SERIAL, $options->serialConsistency);
        $this->assertEquals(15000, $options->pageSize);
        $this->assertEquals(15, $options->timeout);
        $this->assertEquals(array('a', 1, 'b', 2, 'c', 3), $options->arguments);
    }

    public function testReturnsNullValuesWhenRetrievingUndefinedSettingsByName()
    {
        $options = new ExecutionOptions(array());

        $this->assertNull($options->consistency);
        $this->assertNull($options->serialConsistency);
        $this->assertNull($options->pageSize);
        $this->assertNull($options->timeout);
        $this->assertNull($options->arguments);
    }
}
