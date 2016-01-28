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
 * Basic/Simple integration test class to provide common integration test
 * functionality when a simple setup and teardown is required. This class
 * should be used for the majority of tests.
 */
abstract class BasicIntegrationTest extends \PHPUnit_Framework_TestCase {
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
    protected $session;
    /**
     * Version of Cassandra/DSE the session is connected to.
     *
     * @var string
     */
    protected $serverVersion;
    /**
     * Table name prefix being used for the test.
     *
     * @var string
     */
    protected $tableNamePrefix;

    /**
     * Setup the database for the integration tests.
     */
    protected function setUp() {
        // Initialize the database and establish a connection
        $this->integration = new Integration(get_class(), $this->getName(false));
        $this->session = $this->integration->session;
        $this->serverVersion = $this->integration->serverVersion;

        // Assign the table name for the test
        $this->tableNamePrefix = strtolower($this->getName(false));
    }

    /**
     * Teardown the database for the integration tests.
     *
     * @after
     */
    protected function teardown() {
        unset($this->integration);
    }
}
