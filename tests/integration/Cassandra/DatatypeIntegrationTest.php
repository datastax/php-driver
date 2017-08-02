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

/**
 * Datatype integration tests.
 */
class DatatypeIntegrationTest extends DatatypeIntegrationTests {
    /**
     * Scalar data types
     *
     * This test ensures that data types work with all Cassandra's scalar
     * types.
     *
     * @test
     * @dataProvider dataTypes
     */
    public function testDataTypes($type, $values) {
        foreach ($values as $value) {
            $this->createTableInsertAndVerifyValueByIndex($type, $value);
            $this->createTableInsertAndVerifyValueByName($type, $value);
        }
    }

    /**
     * Data provider scalar data types
     */
    public function dataTypes() {
        return array_map(function ($cassandraType) {
            $type = $cassandraType[0];
            $values = array();
            foreach ($cassandraType[1] as $value) {
                $values[] = $value;
            }
            return array($type, $values);
        }, $this->scalarCassandraTypes());
    }

    /**
     * Ensure Decimal/Varint encoding on byte boundaries
     *
     * This test will ensure that the PHP driver is properly encoding Decimal
     * and Varint datatypes for positive values with leading 1's that land on
     * a byte boundary.
     *
     * @test
     * @ticket PHP-70
     */
    public function testByteBoundaryDecimalVarint() {
        // Create the table
        $this->session->execute(
            "CREATE TABLE {$this->tableNamePrefix} " .
            "(key timeuuid PRIMARY KEY, value_decimal decimal, value_varint varint)"
        );

        // Iterate through a few byte boundary positive values
        foreach (range(1, 20) as $i) {
            // Assign the values for the statement
            $key = new Timeuuid();
            $value_varint = pow(2, (8 * $i)) - 1;
            $value_decimal = $value_varint / 100;
            $values = array(
                $key,
                new Decimal($value_decimal),
                new Varint($value_varint)
            );

            // Insert the value into the table
            $this->session->execute(
                "INSERT INTO {$this->tableNamePrefix} (key, value_decimal, value_varint) VALUES (?, ?, ?)",
                array("arguments" => $values)
            );

            // Select the decimal and varint
            $rows = $this->session->execute(
                "SELECT value_decimal, value_varint FROM {$this->tableNamePrefix} WHERE key=?",
                array("arguments" => array($key))
            );

            // Ensure the decimal and varint are valid
            $this->assertCount(1, $rows);
            $row = $rows->first();
            $this->assertNotNull($row);
            $this->assertArrayHasKey("value_decimal", $row);
            $this->assertEquals($values[1], $row["value_decimal"]);
            $this->assertArrayHasKey("value_varint", $row);
            $this->assertEquals($values[2], $row["value_varint"]);
        }
    }
}
