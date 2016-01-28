<?php

namespace Cassandra;

class SetIntegrationTest extends CollectionsIntegrationTest
{
    /**
     * Set with scalar types
     *
     * This test ensures that sets work with all Cassandra's
     * scalar types.
     *
     * @test
     * @dataProvider setWithScalarTypes
     */
    public function testScalarTypes($type, $value) {
        $this->createTableInsertAndVerifyValueByIndex($type, $value);
        $this->createTableInsertAndVerifyValueByName($type, $value);
    }

    /**
     * Data provider for sets with scalar types
     */
    public function setWithScalarTypes() {
        return array_map(function ($cassandraType) {
            $setType = Type::set($cassandraType[0]);
            $set = $setType->create();
            foreach ($cassandraType[1] as $value) {
                $set->add($value);
            }
            return array($setType, $set);
        }, $this->scalarCassandraTypes());
    }

    /**
     * Set with composite types
     *
     * This test ensures that sets work with other nested collections
     * and other composite types such as UDTs and tuples.
     *
     * @test
     * @ticket PHP-62
     * @ticket PHP-57
     * @ticket PHP-58
     * @dataProvider setWithCompositeTypes
     */
    public function testCompositeTypes($type, $value) {
        $this->createTableInsertAndVerifyValueByIndex($type, $value);
        $this->createTableInsertAndVerifyValueByName($type, $value);
    }

    /**
     * Data provider for sets with composite types
     */
    public function setWithCompositeTypes() {
        return array_map(function ($cassandraType) {
            $setType = Type::set($cassandraType[0]);
            $set = $setType->create();
            foreach ($cassandraType[1] as $value) {
                $set->add($value);
            }
            return array($setType, $set);
        }, $this->compositeCassandraTypes());
    }

    /**
     * Set with nested composite types
     *
     * This test ensures that sets work with other nested collections
     * and other composite types such as UDTs and tuples.
     *
     * @test
     * @ticket PHP-62
     * @ticket PHP-57
     * @ticket PHP-58
     * @dataProvider setWithNestedTypes
     */
    public function testNestedTypes($type, $value) {
        $this->createTableInsertAndVerifyValueByIndex($type, $value);
        $this->createTableInsertAndVerifyValueByName($type, $value);
    }

    /**
     * Data provider for sets with nested composite types
     */
    public function setWithNestedTypes() {
        return array_map(function ($cassandraType) {
            $setType = Type::set($cassandraType[0]);
            $set = $setType->create();
            foreach ($cassandraType[1] as $value) {
                $set->add($value);
            }
            return array($setType, $set);
        }, $this->nestedCassandraTypes());
    }

    /**
     * Bind statment with an empty set
     */
    public function testEmpty() {
        $setType = Type::set(Type::int());
        $this->createTableInsertAndVerifyValueByIndex($setType, $setType->create());
        $this->createTableInsertAndVerifyValueByName($setType, $setType->create());
    }

    /**
     * Bind statment with an null set
     */
    public function testNull() {
        $setType = Type::set(Type::int());
        $this->createTableInsertAndVerifyValueByIndex($setType, null);
        $this->createTableInsertAndVerifyValueByName($setType, null);
    }
}
