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

/**
 * Tuple integration tests
 *
 * @cassandra-version-2.1
 */
class TupleIntegrationTest extends \PHPUnit_Framework_TestCase {
    /**
     * Integration test instance (helper class)
     *
     * @var Integration
     */
    private $integration;
    /**
     * Connected database session
     *
     * @var \Cassandra\Session
     */
    private $session;
    /**
     * Table name prefix being used for the test
     *
     * @var string
     */
    private $tableNamePrefix;

    /**
     * Setup the database for the tuple tests
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
     * Teardown the database for the tuple tests
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
     * @return array Valid scalar Cassandra datatypes with test values.
     */
    private function generateScalarValues() {
        return array(
            \Cassandra::TYPE_ASCII => "cassandra",
            \Cassandra::TYPE_BIGINT => new Bigint("9223372036854775807"),
            \Cassandra::TYPE_BLOB => new Blob("blob"),
            \Cassandra::TYPE_BOOLEAN => true,
            \Cassandra::TYPE_DECIMAL => new Decimal("3.14159265359"),
            \Cassandra::TYPE_DOUBLE => 3.14159,
            \Cassandra::TYPE_FLOAT => new Float(3.14159),
            \Cassandra::TYPE_INET => new Inet("127.0.0.1"),
            \Cassandra::TYPE_INT => 123,
            \Cassandra::TYPE_TEXT => "text",
            \Cassandra::TYPE_TIMESTAMP => new Timestamp(123),
            \Cassandra::TYPE_TIMEUUID => new Timeuuid(),
            \Cassandra::TYPE_UUID => new Uuid("03398c99-c635-4fad-b30a-3b2c49f785c2"),
            \Cassandra::TYPE_VARCHAR => "varchar",
//TODO: Re-enable/Uncomment after PHP-70 has been merged
//            \Cassandra::TYPE_VARINT => new Varint("9223372036854775808")
        );
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
        foreach(range(1, $size) as $i) {
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
        $types = array();
        foreach(range(1, $size) as $i) {
            array_push($types, $datatype);
        }
        $tuple = new Tuple($types);
        foreach(range(1, $size) as $i) {
            $tuple->set($i - 1, $value);
        }
        return $tuple;
    }

    private function generateNestedTuple($datatype, $value, $size) {
        // Create the initial tuple (smallest; deepest tuple)
        $tuple = new Tuple(array($datatype));
        $tuple->set(0, $value);

        // Create the nested tuple object
        foreach(range(2, $size) as $i) {
            // Generate the types for the tuple
            $type = array();
            foreach(range(1, $i) as $j) {
                array_push($type, $datatype);
            }
            array_push($type, $tuple->type());

            // Assign the values in the nested tuple
            $nestedTuple = new Tuple($type);
            foreach(range(1, $i) as $j) {
                $nestedTuple->set($j - 1, $value);
            }
            $nestedTuple->set($i, $tuple);
            $tuple = $nestedTuple;
        }

        // Return the completed nested tuple
        return $tuple;
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
        // Generate and create the table
        $query = "CREATE TABLE " .
            $this->tableNamePrefix . "_" . $datatype . "_" . $size .
            " (key timeuuid PRIMARY KEY, value " .
            $this->generateTupleCQL($datatype, $size, $isNested) . ")";
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
        $query = "INSERT INTO " . $this->tableNamePrefix . "_" . $datatype . "_" . $size . " (key, value) VALUES (?, ?)";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => $values));
        $this->session->execute($statement, $options);

        // Return the key for asserting tuple
        return $key;
    }

    /**
     * Make assertions on each element in the nested tuple.
     *
     * @param $tuple Tuple to validate
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
        // Select the tuple
        $query = "SELECT value FROM " . $this->tableNamePrefix . "_" . $datatype . "_" . $size . " WHERE key=?";
        $statement = new SimpleStatement($query);
        $options = new ExecutionOptions(array("arguments" => array($key)));
        $rows = $this->session->execute($statement, $options);

        // Ensure the tuple collection is valid
        $this->assertEquals($rows->count(), 1);
        $row = $rows->first();
        $this->assertNotNull($row);
        $this->assertArrayHasKey("value", $row);
        $tuple = $row["value"];
        // Handle TEXT alias (VARCHAR)
        $this->assertEquals($tuple->type(),
            $this->generateTupleCQL(($datatype == \Cassandra::TYPE_TEXT ? \Cassandra::TYPE_VARCHAR : $datatype), $size, $isNested, false));

        // Assert the elements in the tuple
        if ($isNested) {
            $this->assertNestedTupleElements($tuple, $value);
        } else {
            foreach ($tuple as $element) {
                $this->assertEquals($element, $value);
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
}