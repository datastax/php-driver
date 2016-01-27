<?php

namespace Cassandra;

class MapIntegrationTest extends CollectionsIntegrationTest
{
    /**
     * Map with scalar types
     *
     * This test ensures that maps work with all Cassandra's
     * scalar types.
     *
     * @test
     * @dataProvider mapWithScalarTypes
     */
    public function testScalarCassandraTypes($type, $value) {
        $this->createTableInsertAndVerifyValueByIndex($type, $value);
        $this->createTableInsertAndVerifyValueByName($type, $value);
    }

    /**
     * Map with composite types
     *
     * This test ensures that maps work with other nested collections
     * and other composite types such as UDTs and tuples.
     *
     * @test
     * @ticket PHP-62
     * @ticket PHP-57
     * @ticket PHP-58
     * @dataProvider mapWithCompositeTypes
     */
    public function testCompositeCassandraTypes($type, $value) {
        $this->createTableInsertAndVerifyValueByIndex($type, $value);
        $this->createTableInsertAndVerifyValueByName($type, $value);
    }

    /**
     * Bind statment with an empty map
     */
    public function testNull() {
        $mapType = Type::map(Type::int(), Type::int());
        $this->createTableInsertAndVerifyValueByIndex($mapType, $mapType->create());
        $this->createTableInsertAndVerifyValueByName($mapType, $mapType->create());
    }

    /**
     * Bind statment with an null map
     */
    public function testEmpty() {
        $mapType = Type::map(Type::int(), Type::int());
        $this->createTableInsertAndVerifyValueByIndex($mapType, null);
        $this->createTableInsertAndVerifyValueByName($mapType, null);
    }

    public function mapWithScalarTypes() {
        $mapKeyTypes = array_map(function ($cassandraType) {
            $mapType = Type::map($cassandraType[0], Type::int());
            $values = $cassandraType[1];
            $map = $mapType->create();
            for ($i = 0; $i < count($cassandraType[1]); $i++) {
                $map->set($values[$i], $i);
            }
            return array($mapType, $map);
        }, $this->scalarCassandraTypes());

        $mapValueTypes = array_map(function ($cassandraType) {
            $mapType = Type::map(Type::int(), $cassandraType[0]);
            $values = $cassandraType[1];
            $map = $mapType->create();
            for ($i = 0; $i < count($cassandraType[1]); $i++) {
                $map->set($i, $values[$i]);
            }
            return array($mapType, $map);
        }, $this->scalarCassandraTypes());

        return array_merge($mapKeyTypes, $mapValueTypes);
    }

    public function mapWithCompositeTypes() {
        $mapKeyTypes = array_map(function ($cassandraType) {
            $mapType = Type::map($cassandraType[0], Type::int());
            $values = $cassandraType[1];
            $map = $mapType->create();
            for ($i = 0; $i < count($cassandraType[1]); $i++) {
                $map->set($values[$i], $i);
            }
            return array($mapType, $map);
        }, $this->compositeCassandraTypes());

        $mapValueTypes = array_map(function ($cassandraType) {
            $mapType = Type::map(Type::int(), $cassandraType[0]);
            $values = $cassandraType[1];
            $map = $mapType->create();
            for ($i = 0; $i < count($cassandraType[1]); $i++) {
                $map->set($i, $values[$i]);
            }
            return array($mapType, $map);
        }, $this->compositeCassandraTypes());

        return array_merge($mapKeyTypes, $mapValueTypes);
    }
}
