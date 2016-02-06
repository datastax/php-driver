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
}