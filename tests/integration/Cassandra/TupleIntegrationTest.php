<?php

namespace Cassandra;

/**
 * Copyright 2015-2016 DataStax, Inc.
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
use Cassandra\Type\UserType;

/**
 * Tuple integration tests.
 *
 * @cassandra-version-2.1
 */
class TupleIntegrationTest extends \PHPUnit_Framework_TestCase {
    /**
     * Integration test instance (helper class).
     *
     * @var Integration
     */
    private $integration;
    /**
     * Connected database session.
     *
     * @var \Cassandra\Session
     */
    private $session;
    /**
     * Table name prefix being used for the test.
     *
     * @var string
     */
    private $tableNamePrefix;

    /**
     * Setup the database for the tuple tests.
     *
     * @before
     */
    public function setupTest() {
        // Initialize the database and establish a connection
        $this->integration = new Integration(get_class(), $this->getName());
        $this->session = $this->integration->session;

        // Assign the table name for the test
        $this->tableNamePrefix = strtolower($this->getName());
    }

    /**
     * Teardown the database for the tuple tests.
     *
     * @after
     */
    public function teardownTest() {
        unset($this->integration);
    }

    /**
     * Generate an array of valid Cassandra datatypes with values that can be
     * used for testing.
     *
     * @param $isNullTest bool True if scalar values are null; false otherwise
     * @return array Valid scalar Cassandra datatypes with test values
     */
    private function generateScalarValues($isNullTest = false) {
        return [
            \Cassandra::TYPE_ASCII => ($isNullTest ? null : "cassandra"),
            \Cassandra::TYPE_BIGINT => ($isNullTest ? null : new Bigint("9223372036854775807")),
            \Cassandra::TYPE_BLOB => ($isNullTest ? null : new Blob("blob")),
            \Cassandra::TYPE_BOOLEAN => ($isNullTest ? null : true),
            \Cassandra::TYPE_DECIMAL => ($isNullTest ? null : new Decimal("3.14159265359")),
            \Cassandra::TYPE_DOUBLE => ($isNullTest ? null : 3.14159),
            \Cassandra::TYPE_FLOAT => ($isNullTest ? null : new Float(3.14159)),
            \Cassandra::TYPE_INET => ($isNullTest ? null : new Inet("127.0.0.1")),
            \Cassandra::TYPE_INT => ($isNullTest ? null : 123),
            \Cassandra::TYPE_TEXT => ($isNullTest ? null : "text"),
            \Cassandra::TYPE_TIMESTAMP => ($isNullTest ? null : new Timestamp(123)),
            \Cassandra::TYPE_TIMEUUID => ($isNullTest ? null : new Timeuuid()),
            \Cassandra::TYPE_UUID => ($isNullTest ? null : new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2")),
            \Cassandra::TYPE_VARCHAR => ($isNullTest ? null : "varchar"),
//TODO: Re-enable/Uncomment after PHP-70 has been merged
//            \Cassandra::TYPE_VARINT => ($isNullTest ? null : new Varint("9223372036854775808"))
        ];
    }

    /**
     * Generate an array of valid Cassandra composite datatypes with scalar
     * values that can be used for testing.
     *
     * @param $size Number of elements in the collection
     * @return array Valid composite Cassandra datatypes from scalar test
     *               values
     */
    private function generateCompositeValues($size) {
        // Create the composite types from the scalar values
        $compositeValues = array();
        foreach ($this->generateScalarValues() as $datatype => $value) {
            $map = new Map(\Cassandra::TYPE_INT, $datatype);
            $set = new Set($datatype);
            foreach (range(1, $size) as $i) {
                $map->set($i, $value);
                $this->assertTrue($set->add($value));
            }

            // Add the composite types to the array of values
            array_push($compositeValues, array($datatype => array($map, $set)));
        }

        // Return the composite values made from valid scalar values
        return $compositeValues;
    }

    /**
     * Generate the elements for a tuple.
     *
     * @param $datatype Cassandra datatype to use for elements
     * @param $size Number of elements to generate
     * @return string Elements for tuple
     */
    private function generateTupleElements($datatype, $size) {
        $datatypes = "";
        foreach (range(1, $size) as $i) {
            if ($i > 1) {
                $datatypes .= ", ";
            }
            $datatypes .= $datatype;
        }
        return $datatypes;
    }

    /**
     * Generate the CQL for a tuple.
     *
     * @param $datatype Cassandra datatype to use for elements
     * @param $size Number of elements to generate
     * @param $isNested bool True if tuple is nested; false otherwise
     * @param bool $isFrozen True if frozen; false otherwise (DEFAULT: false)
     * @return string CQL for tuple
     */
    private function generateTupleCQL($datatype, $size, $isNested, $isFrozen = true) {
        $cql = ($isFrozen ? "frozen<tuple<" : "tuple<");
        if ($isNested && $size > 1) {
            $tuple = $this->generateTupleElements($datatype, $size) . ", ";
            foreach (range($size - 1, 1, -1) as $i) {
                if ($i < $size - 1) {
                    $tuple .= ", ";
                }
                $tuple .= "tuple<" . $this->generateTupleElements($datatype, $i);
            }
            $cql .= $tuple . ">" . str_repeat(">", $size - 2);
        } else {
            $cql .= $this->generateTupleElements($datatype, $size);
        }
        $cql .= ($isFrozen ? ">>" : ">");
        return $cql;
    }

    /**
     * Generate a tuple
     *
     * @param $datatype Cassandra datatype for each element in the tuple
     * @param $value Value for each element in the tuple
     * @param $size Number of elements in the tuple
     * @return \Cassandra\Tuple Value assigned tuple
     */
    private function generateTuple($datatype, $value, $size) {
        // Create the tuple type
        $types = array();
        foreach (range(1, $size) as $i) {
            if ($value instanceof Map || $value instanceof Set) {
                array_push($types, $value->type());
            } else {
                array_push($types, $datatype);
            }
        }
        $tuple = new Tuple($types);

        // Assign the values to the elements of the tuple
        foreach (range(1, $size) as $i) {
            $tuple->set($i - 1, $value);
        }
        return $tuple;
    }

    /**
     * Generate a nested tuple.
     *
     * @param $datatype Cassandra datatype for each element in the nested tuple
     * @param $value Value for each element in the nested tuple
     * @param $depth Depth of nested tuple with decaying number of elements
     * @return \Cassandra\Tuple Value assigned nested tuple
     */
    private function generateNestedTuple($datatype, $value, $depth) {
        // Create the initial tuple (smallest; deepest tuple)
        $datatype = $this->getCQLTypes($datatype, $value)["datatype"];
        $tuple = new Tuple(array($datatype));
        $tuple->set(0, $value);

        // Create the nested tuple object
        foreach (range(2, $depth) as $i) {
            // Generate the types for the tuple
            $type = array();
            foreach (range(1, $i) as $j) {
                array_push($type, $datatype);
            }
            array_push($type, $tuple->type());

            // Assign the values in the nested tuple
            $nestedTuple = new Tuple($type);
            foreach (range(1, $i) as $j) {
                $nestedTuple->set($j - 1, $value);
            }
            $nestedTuple->set($i, $tuple);
            $tuple = $nestedTuple;
        }

        // Return the completed nested tuple
        return $tuple;
    }

    /**
     * Get/Update the CQL datatype based on the value being a composite or
     * scalar datatype. This function will also create a suffix that should be
     * used for tables when creating and reading/writing data in order to keep
     * tables run in a test distinct from each other.
     *
     * @param $datatype Current datatype for the value
     * @param $value \Cassandra\Value Value to check for composite types
     * @return array datatype - Datatype for use in CQL queries
     *               table_suffix - Suffix to keep tables distinct
     */
    private function getCQLTypes($datatype, $value) {
        // Determine if the datatype should be updated to reflect the composite
        $cqlDatatype = $datatype;
        $cqlTableSuffix = $datatype;
        if ($value instanceof Map || $value instanceof Set) {
            $cqlDatatype = $value->type();
            $cqlTableSuffix = $cqlTableSuffix . "_" . substr($cqlDatatype, 0, 3);
        }

        // Return the CQL types
        return array(
            "datatype" => $cqlDatatype,
            "table_suffix" => $cqlTableSuffix
        );
    }

    /**
     * Create a table and insert a tuple based on the value and datatype into
     * Cassandra.
     *
     * @param $datatype Cassandra datatype for each element in the tuple
     * @param $value Value for each element in the tuple
     * @param $size Number of elements to generate
     * @return Timeuuid Key used during insert
     * @param $isNested bool True if tuple is nested; false otherwise
     *                       (DEFAULT: false)
     */
    private function insertTuple($datatype, $value, $size, $isNested = false) {
        // Get the CQL datatype (handles composite types)
        $cqlTypes = $this->getCQLTypes($datatype, $value);
        $cqlDatatype = $cqlTypes["datatype"];
        $cqlTableType = $cqlTypes["table_suffix"];

        // Generate and create the table
        $query = "CREATE TABLE " .
            $this->tableNamePrefix . "_" . $cqlTableType . "_" . $size .
            " (key timeuuid PRIMARY KEY, value " .
            $this->generateTupleCQL($cqlDatatype, $size, $isNested) . ")";
        $this->session->execute(new SimpleStatement($query));

        // Prepare the value for insert into the table
        $tuple = null;
        if ($isNested) {
            $tuple = $this->generateNestedTuple($datatype, $value, $size);
        } else {
            $tuple = $this->generateTuple($datatype, $value, $size);
        }

        // Assign the values for the statement
        $key = new Timeuuid();
        $values = array(
            $key,
            $tuple
        );

        // Insert the value into the table
        $query = "INSERT INTO " . $this->tableNamePrefix . "_" . $cqlTableType . "_" . $size . " (key, value) VALUES (?, ?)";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => $values));
        $this->session->execute($statement, $options);

        // Return the key for asserting tuple
        return $key;
    }

    /**
     * Make assertions on each element in the nested tuple.
     *
     * @param $tuple \Cassandra\Tuple Tuple object to validate
     * @param $value Value to assert against each element of the tuple
     */
    private function assertNestedTupleElements($tuple, $value) {
        foreach ($tuple as $element) {
            if ($element instanceof Tuple) {
                $this->assertNestedTupleElements($element, $value);;
            } else {
                $this->assertEquals($element, $value);
            }
        }
    }

    /**
     * Make assertions on each element in the tuple.
     *
     * @param $key Key to use to select tuple value
     * @param $datatype Cassandra datatype to assert against each element in
     *                  the tuple
     * @param $value Value to assert against each element in the tuple
     * @param $size Number of elements in the tuple
     * @param $isNested bool True if tuple is nested; false otherwise
     *                       (DEFAULT: false)
     */
    private function assertTuple($key, $datatype, $value, $size, $isNested = false) {
        // Get the CQL datatype (handles composite types)
        $cqlTypes = $this->getCQLTypes($datatype, $value);
        $cqlDatatype = $cqlTypes["datatype"];
        $cqlTableType = $cqlTypes["table_suffix"];

        //Handle TEXT alias (VARCHAR)
        $cqlDatatype = str_replace(\Cassandra::TYPE_TEXT, \Cassandra::TYPE_VARCHAR, $cqlDatatype);

        // Select the tuple
        $query = "SELECT value FROM " . $this->tableNamePrefix . "_" . $cqlTableType . "_" . $size . " WHERE key=?";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => array($key)));
        $rows = $this->session->execute($statement, $options);

        // Ensure the tuple collection is valid
        $this->assertCount(1, $rows);
        $row = $rows->first();
        $this->assertNotNull($row);
        $this->assertArrayHasKey("value", $row);
        $tuple = $row["value"];
        $this->assertInstanceOf('Cassandra\Tuple', $tuple);
        $this->assertEquals($this->generateTupleCQL($cqlDatatype, $size, $isNested, false), $tuple->type());

        // Assert the elements in the tuple
        if ($isNested) {
            $this->assertNestedTupleElements($tuple, $value);
        } else {
            foreach ($tuple as $element) {
                $this->assertEquals($value, $element);
            }
        }
    }

    /**
     * Tuples using scalar/simple datatypes
     *
     * This test will ensure that the PHP driver supports the tuples collection
     * with all PHP driver supported scalar/simple datatypes.
     *
     * @test
     * @ticket PHP-58
     */
    public function scalarDatatypes() {
        $sizes = array(1, 2, 3, 5, 37, 73, 74, 877);
        foreach ($this->generateScalarValues() as $datatype => $value) {
            foreach ($sizes as $size) {
                $key = $this->insertTuple($datatype, $value, $size);
                $this->assertTuple($key, $datatype, $value, $size);
            }
        }
    }

    /**
     * Nested tuples using scalar/simple datatypes
     *
     * This test will ensure that the PHP driver supports the tuples collection
     * with all PHP driver supported scalar/simple datatypes in a nested tuple.
     *
     * @test
     * @ticket PHP-58
     */
    public function nestedScalarDatatypes() {
        $depths = array(4, 5, 37);
        foreach ($this->generateScalarValues() as $datatype => $value) {
            foreach ($depths as $depth) {
                $key = $this->insertTuple($datatype, $value, $depth, true);
                $this->assertTuple($key, $datatype, $value, $depth, true);
            }
        }
    }

    /**
     * Tuples using composite/collection datatypes.
     *
     * This test will ensure that the PHP driver supports the tuples collection
     * with all the PHP driver supported composite/collection datatypes.
     *
     * NOTE: This test does not utilize tuples composite since they are used in
     *       the nestedScalarDatatypes test; anything here would be redundant.
     *
     * @test
     * @ticket PHP-58
     */
    public function compositeDatatypes() {
        // Keep the sizes relatively small for speed of test and memory usage
        $sizes = array(1, 2, 3, 5);
        foreach ($sizes as $size) {
            foreach ($this->generateCompositeValues($size) as $composites) {
                foreach ($composites as $datatype => $values) {
                    foreach ($values as $value) {
                        $key = $this->insertTuple($datatype, $value, $size);
                        $this->assertTuple($key, $datatype, $value, $size);
                    }
                }
            }
        }
    }

    /**
     * Nested tuples using composite/collection datatypes.
     *
     * This test will ensure that the PHP driver supports the tuples collection
     * with all the PHP driver supported composite/collection datatypes in a
     * nested tuple.
     *
     * NOTE: This test does not utilize tuples composite since they are used in
     *       the nestedScalarDatatypes test; anything here would be redundant.
     *
     * @test
     * @ticket PHP-58
     */
    public function compositeNestedDatatypes() {
        // Keep the sizes relatively small for speed of test and memory usage
        $depths = array(3, 5);
        foreach ($depths as $depth) {
            foreach ($this->generateCompositeValues($depth) as $composites) {
                foreach ($composites as $datatype => $values) {
                    foreach ($values as $value) {
                        $key = $this->insertTuple($datatype, $value, $depth, true);
                        $this->assertTuple($key, $datatype, $value, $depth, true);
                    }
                }
            }
        }
    }

    /**
     * Tuple using a nested user type.
     *
     * This test will ensure that the PHP driver supports the tuples collection
     * with user types.
     *
     * @test
     * @ticket PHP-57
     */
    public function usertype() {
        // Create the user types
        $this->session->execute(new SimpleStatement(UserTypeIntegrationTest::PHONE_USER_TYPE_CQL));
        $this->session->execute(new SimpleStatement(UserTypeIntegrationTest::ADDRESS_USER_TYPE_CQL));

        // Create the table
        $query = "CREATE TABLE " . $this->tableNamePrefix .
            " (key timeuuid PRIMARY KEY, value " .
            "frozen<tuple<address>>)";
        $this->session->execute(new SimpleStatement($query));

        // Generate a valid address user type and assign it to a tuple
        $address = UserTypeIntegrationTest::generateAddressValue();
        $tuple = new Tuple(array($address->type()));
        $tuple->set(0, $address);

        // Assign the values for the statement
        $key = new Timeuuid();
        $values = array(
            $key,
            $tuple
        );

        // Insert the value into the table
        $query = "INSERT INTO " . $this->tableNamePrefix . " (key, value) VALUES (?, ?)";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => $values));
        $this->session->execute($statement, $options);

        // Select the tuple
        $query = "SELECT value FROM " . $this->tableNamePrefix . " WHERE key=?";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => array($key)));
        $rows = $this->session->execute($statement, $options);

        // Ensure the tuple collection is valid
        $this->assertCount(1, $rows);
        $row = $rows->first();
        $this->assertNotNull($row);
        $this->assertArrayHasKey("value", $row);
        $tuple = $row["value"];
        $this->assertInstanceOf('Cassandra\Tuple', $tuple);
        $this->assertCount(1, $tuple);

        // Verify the value can be read from the table
        UserTypeIntegrationTest::assertAddressValue($tuple->get(0));
    }

    /**
     * Invalid datatypes for Tuples
     *
     * This test will ensure that an exception will occur when an invalid
     * datatype is used inside a tuple; issues from the server.
     *
     * @test
     * @ticket PHP-58
     * @expectedException \Cassandra\Exception\InvalidQueryException
     */
    public function invalidType() {
        $counterType = array(\Cassandra::TYPE_COUNTER => new Bigint(123));
        foreach ($counterType as $datatype => $value) {
            $this->insertTuple($datatype, $value, 1);
        }
    }

    /**
     * NULL values in datatypes for Tuples.
     *
     * This test will ensure that data can be inserted into the table when NULL
     * values are present in the tuple. Both scalar and nested scalar datatypes
     * will be tested.
     *
     * @test
     * @ticket PHP-58
     */
    public function nullValueScalarDatatypes() {
        // Non-nested
        $sizes = array(1, 2, 3, 5);
        foreach ($this->generateScalarValues(true) as $datatype => $value) {
            foreach ($sizes as $size) {
                $key = $this->insertTuple($datatype, $value, $size);
                $this->assertTuple($key, $datatype, $value, $size);
            }
        }

        // Nested
        $depths = array(4, 6);
        foreach ($this->generateScalarValues(true) as $datatype => $value) {
            foreach ($depths as $depth) {
                $key = $this->insertTuple($datatype, $value, $depth, true);
                $this->assertTuple($key, $datatype, $value, $depth, true);
            }
        }
    }
}