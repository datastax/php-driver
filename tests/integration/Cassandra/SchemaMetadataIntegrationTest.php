<?php

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

namespace Cassandra;

/**
 * Schema metadata integration tests.
 */
class SchemaMetadataIntegrationTest extends BasicIntegrationTest {
    /**
     * Schema snapshot associated with the $this->session connection.
     *
     * @var \Cassandra\Schema
     */
    private $schema;

    /**
     * Setup the schema metadata for the schema metadata tests.
     */
    public function setUp() {
        // Process parent setup steps
        parent::setUp();

        // Initialize the session schema metadata snapshot
        $this->schema = $this->session->schema();
    }

    protected static function generateKeyspaceName($prefix) {
        return substr(uniqid($prefix), 0, 48);
    }

    protected function createKeyspace($keyspaceName, $replicationFactor = 1) {
        $statement = new SimpleStatement(
            "CREATE KEYSPACE $keyspaceName ".
            "WITH REPLICATION = { 'class' : 'SimpleStrategy', 'replication_factor' : $replicationFactor }"
        );
        $this->session->execute($statement);
    }

    protected function createKeyspaceWithSchema($keyspaceName, $tableSchemas) {
        $this->createKeyspace($keyspaceName);
        foreach ($tableSchemas as $tableName => $tableSchema) {
            $query = sprintf("CREATE TABLE $keyspaceName.$tableName (%s, PRIMARY KEY(%s))",
                implode(", ",
                    array_map(function ($key, $value) { return "$key $value"; },
                    array_keys($tableSchema), array_values($tableSchema))),
                implode(", ",
                    array_filter(array_keys($tableSchema),
                    function($columnName) { return strpos($columnName, "key") === 0; }))
            );
            $this->session->execute(new SimpleStatement($query));
        }
    }

    /**
     * Schema metadata support is available; basic test.
     *
     * This test will ensure that the PHP driver supports schema metadata.
     */
    public function testBasicSchemaMetadata() {
        // Ensure the test class session connection has schema metadata
        $this->assertGreaterThan(0, count($this->schema));

        // Ensure the test class session contains the test keyspace
        $this->assertArrayHasKey($this->keyspaceName, $this->schema->keyspaces());
        $keyspace = $this->schema->keyspace($this->keyspaceName);
    }

    /**
     * Schema metadata support can be disabled.
     *
     * This test will ensure that the PHP driver supports the ability to enable
     * and disable the schema metadata when creating a session object.
     *
     * @test
     * @ticket PHP-61
     */
    public function testDisableSchemaMetadata() {
        // Create a new session with schema metadata disabled
        $cluster   = \Cassandra::cluster()
            ->withContactPoints(Integration::IP_ADDRESS) //TODO: Need to use configured value when support added
            ->withSchemaMetadata(false)
            ->build();
        $session   = $cluster->connect();

        // Get the schema from the new session
        $schema = $session->schema();

        // Ensure the new session has no schema metadata
        $this->assertCount(0, $schema->keyspaces());
        $this->assertNotEquals($this->schema->keyspaces(), $schema->keyspaces());
    }

    /**
     * Enumerate over keyspaces in schema metadata.
     *
     * This test ensures that driver correctly enumerates over keyspace
     * metadata.
     *
     * @test
     */
    public function testEnumerateKeyspaces() {
        $keyspaceNames =  array(
            self::generateKeyspaceName("enumerate_ks0"),
            self::generateKeyspaceName("enumerate_ks1"),
            self::generateKeyspaceName("enumerate_ks2"),
            "system",
        );

        foreach ($keyspaceNames as $keyspaceName) {
            if (strpos($keyspaceName, "system") === 0) continue;
            $this->createKeyspace($keyspaceName);
        }

        $count = 0;
        foreach($this->session->schema()->keyspaces() as $keyspace) {
            if (in_array($keyspace->name(), $keyspaceNames)) {
                $count++;
            }
        }

        $this->assertEquals($count, count($keyspaceNames));
    }

    /**
     * Get keyspace from schema metadata using keyspace name.
     *
     * This test ensures that the driver is able to access keyspace metadata by
     * name.
     *
     * @test
     */
    public function testGetKeyspaceByName() {
        $keyspaceNames =  array(
            self::generateKeyspaceName("by_name_ks0"),
            self::generateKeyspaceName("by_name_ks1"),
            self::generateKeyspaceName("by_name_ks2"),
            "system",
        );

        foreach ($keyspaceNames as $keyspaceName) {
            if (strpos($keyspaceName, "system") === 0) continue;
            $this->createKeyspace($keyspaceName);
        }

        $count = 0;
        foreach($keyspaceNames as $keyspaceName) {
            $keyspace = $this->session->schema()->keyspace($keyspaceName);
            if (isset($keyspace)) {
                $count++;
            }
        }

        $this->assertEquals($count, count($keyspaceNames));
    }

    /**
     * Enumerate over tables and columns in schema metadata.
     *
     * This test ensures that driver correctly enumerates over table and column
     * metadata.
     *
     * @test
     */
    public function testEnumerateTablesAndColumns() {
        $keyspaceName = self::generateKeyspaceName("enumerate");

        $tableSchemas = array(
            "table_int_varchar" => array("key" => "int", "value" => "varchar"),
            "table_varchar_bigint" => array("key" => "varchar", "value" => "bigint"),
            "table_varchar_map" => array("key" => "varchar", "value" => "map<bigint, varchar>")
        );

        $this->createKeyspaceWithSchema($keyspaceName, $tableSchemas);

        $keyspace = $this->session->schema()->keyspace($keyspaceName);

        $this->assertEquals(count($tableSchemas), count($keyspace->tables()));
        foreach($keyspace->tables() as $table) {
            $tableSchema = $tableSchemas[$table->name()];
            $this->assertEquals(count($tableSchema), count($table->columns()));
            foreach ($table->columns() as $column) {
                $columnType = $tableSchema[$column->name()];
                $this->assertEquals($columnType, (string)$column->type());
            }
        }
    }

    /**
     * Get tables and columns from schema metadata using their names.
     *
     * This test ensures that the driver is able to access table and column
     * metadata by name.
     *
     * @test
     */
    public function testGetTableAndColumnByName() {
        $keyspaceName = self::generateKeyspaceName("by_name");

        $tableSchemas = array(
            "table_int_varchar" => array("key" => "int", "value" => "varchar"),
            "table_varchar_bigint" => array("key" => "varchar", "value" => "bigint"),
            "table_varchar_map" => array("key" => "varchar", "value" => "map<bigint, varchar>")
        );

        $this->createKeyspaceWithSchema($keyspaceName, $tableSchemas);

        $keyspace = $this->session->schema()->keyspace($keyspaceName);

        $this->assertEquals(count($tableSchemas), count($keyspace->tables()));

        foreach ($tableSchemas as $tableName => $tableSchema) {
            $table = $keyspace->table($tableName);
            $this->assertEquals(count($tableSchema), count($table->columns()));
            foreach ($tableSchema as $columnName => $columnType) {
                $column = $table->column($columnName);
                $this->assertEquals($columnType, (string)$column->type());
            }
        }
    }

    /**
     * Get index options metadata from a column
     *
     * This test ensures that index options metadata are properly returned from
     * an indexed column.
     *
     * @test
     */
    public function testGetColumnIndexOptions() {
        $statement = new SimpleStatement(
            "CREATE TABLE {$this->tableNamePrefix}_with_index (key int PRIMARY KEY, value map<text, frozen<map<int, int>>>)"
        );
        $this->session->execute($statement);

        $keyspace = $this->session->schema()->keyspace($this->keyspaceName);
        $this->assertNotNull($keyspace);

        $table = $keyspace->table("{$this->tableNamePrefix}_with_index");
        $this->assertNotNull($table);

        $indexOptions = $table->column("value")->indexOptions();
        $this->assertNull($indexOptions);

        $statement = new SimpleStatement(
            "CREATE INDEX ON {$this->tableNamePrefix}_with_index (value)"
        );
        $this->session->execute($statement);

        $keyspace = $this->session->schema()->keyspace($this->keyspaceName);
        $this->assertNotNull($keyspace);

        $table = $keyspace->table("{$this->tableNamePrefix}_with_index");
        $this->assertNotNull($table);

        $indexOptions = $table->column("value")->indexOptions();
        $this->assertNotNull($indexOptions);
        $this->assertInstanceOf('Cassandra\Map', $indexOptions);
    }

    /**
     * Schema metadata data with null fields.
     *
     * This test ensures that table and column metadata with null fields
     * are returned correctly.
     *
     * @test
     */
    public function testSchemaMetadataWithNullFields() {
        $statement = new SimpleStatement(
            "CREATE TABLE {$this->tableNamePrefix}_null_comment (key int PRIMARY KEY, value int)"
        );
        $this->session->execute($statement);

        $keyspace = $this->session->schema()->keyspace($this->keyspaceName);
        $table = $keyspace->table("{$this->tableNamePrefix}_null_comment");
        $this->assertNull($table->comment());

        $column = $table->column("value");
        $this->assertNull($column->indexName());
    }

    /**
     * Schema metadata data with deeply nested collection.
     *
     * This test ensures that the validator parser correctly parses and builds
     * columns with deeply nested collection types.
     *
     * @test
     * @ticket PHP-62
     */
    public function testSchemaMetadataWithNestedColumnTypes() {
        $statement = new SimpleStatement(
            "CREATE TABLE {$this->tableNamePrefix}_nested1 (key int PRIMARY KEY, value map<frozen<list<varchar>>, varchar>)"
        );
        $this->session->execute($statement);

        $statement = new SimpleStatement(
            "CREATE TABLE {$this->tableNamePrefix}_nested2 (key int PRIMARY KEY, value map<varchar, frozen<list<varchar>>>)"
        );
        $this->session->execute($statement);

        $statement = new SimpleStatement(
            "CREATE TABLE {$this->tableNamePrefix}_nested3 (key int PRIMARY KEY, value list<frozen<map<varchar, frozen<set<varchar>>>>>)"
        );
        $this->session->execute($statement);

        $keyspace = $this->session->schema()->keyspace($this->keyspaceName);

        $table1 = $keyspace->table("{$this->tableNamePrefix}_nested1");
        $this->assertEquals((string)$table1->column("value")->type(), "map<list<varchar>, varchar>");

        $table2 = $keyspace->table("{$this->tableNamePrefix}_nested2");
        $this->assertEquals((string)$table2->column("value")->type(), "map<varchar, list<varchar>>");

        $table3 = $keyspace->table("{$this->tableNamePrefix}_nested3");
        $this->assertEquals((string)$table3->column("value")->type(), "list<map<varchar, set<varchar>>>");
    }
}
