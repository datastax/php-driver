<?php

namespace Cassandra;

class CollectionIntegrationTest extends CollectionsTestCase
{
    /**
     * List with scalar types
     *
     * This test ensures that lists work with all Cassandra's
     * scalar types.
     *
     * @test
     * @dataProvider collectionWithScalarTypes
     */
    public function testScalarCassandraTypes($type, $value) {
        $this->createTableInsertAndVerifyValueByIndex($type, $value);
        $this->createTableInsertAndVerifyValueByName($type, $value);
    }

    /**
     * List with composite types
     *
     * This test ensures that lists work with other nested collections
     * and other composite types such as UDTs and tuples.
     *
     * @test
     * @ticket PHP-62
     * @ticket PHP-57
     * @ticket PHP-58
     * @dataProvider collectionWithCompositeTypes
     */
    public function testCompositeCassandraTypes($type, $value) {
        $this->createTableInsertAndVerifyValueByIndex($type, $value);
        $this->createTableInsertAndVerifyValueByName($type, $value);
    }

    /**
     * Bind statment with an empty list
     *
     * @test
     */
    public function testEmpty() {
        $listType = Type::Collection(Type::int());
        $this->createTableInsertAndVerifyValueByIndex($listType, $listType->create());
        $this->createTableInsertAndVerifyValueByName($listType, $listType->create());
    }

    /**
     * Bind statement a null list
     *
     * @test
     */
    public function testNull() {
        $listType = Type::Collection(Type::int());
        $this->createTableInsertAndVerifyValueByIndex($listType, null);
        $this->createTableInsertAndVerifyValueByName($listType, null);
    }

    public function collectionWithScalarTypes() {
        return array_map(function ($cassandraType) {
            $listType = Type::collection($cassandraType[0]);
            $list = $listType->create();
            foreach ($cassandraType[1] as $value) {
                $list->add($value);
            }
            return array($listType, $list);
        }, $this->scalarCassandraTypes());
    }

    public function collectionWithCompositeTypes() {
        return array_map(function ($cassandraType) {
            $listType = Type::collection($cassandraType[0]);
            $list = $listType->create();
            foreach ($cassandraType[1] as $value) {
                $list->add($value);
            }
            return array($listType, $list);
        }, $this->compositeCassandraTypes());
    }
}
