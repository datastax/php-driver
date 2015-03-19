<?php

namespace Cassandra;

class SetTest extends \PHPUnit_Framework_TestCase
{
    public function testContainsUniqueValues()
    {
        $set = new Set('varint');
        $this->assertEquals(0, count($set));
        $set->add(new Varint('123'));
        $this->assertEquals(1, count($set));
        $set->add(new Varint('123'));
        $this->assertEquals(1, count($set));
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Unsupported type 'some custom type'
     */
    public function testSupportsOnlyCassandraTypes()
    {
        new Set('some custom type');
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testSupportsAllCassandraTypes($type)
    {
        new Set($type);
    }

    /**
     * @dataProvider cassandraTypes
     */
    public function testReturnsItsType($type)
    {
        $set = new Set($type);
        $this->assertEquals($type, $set->type());
    }

    public function cassandraTypes()
    {
        return array(
            array("ascii"),
            array("bigint"),
            array("blob"),
            array("boolean"),
            array("counter"),
            array("decimal"),
            array("double"),
            array("float"),
            array("int"),
            array("text"),
            array("timestamp"),
            array("uuid"),
            array("varchar"),
            array("varint"),
            array("timeuuid"),
            array("inet"),
        );
    }

    /**
     * @expectedException         InvalidArgumentException
     * @expectedExceptionMessage  Expected an instance of Cassandra\Varint, an instance of Cassandra\Decimal given
     */
    public function testValidatesTypesOfElements()
    {
        $set = new Set('varint');
        $set->add(new Decimal('123'));
    }

    /**
     * @dataProvider sampleNumbers
     */
    public function testSupportsIteration($numbers)
    {
        $set = new Set('int');

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
        $set = new Set('int');

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
        $set = new Set('int');

        foreach ($numbers as $number) {
            $set->add($number);
        }

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
}
