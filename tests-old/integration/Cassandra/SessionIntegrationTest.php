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
 * Session integration tests
 */
class SessionIntegrationTest extends BasicIntegrationTest {

    /**
     * Ensure the driver does not crash connecting to a non-existent server
     *
     * This test will ensure that when attempting to connect to a non-existent
     * server instance asynchronously, the driver will not crash.
     *
     * @jira_ticket PHP-144
     * @test_category connection
     * @since 1.3.0
     * @excpected_result Driver will not connect and continue to function
     */
    public function testAsyncConnectionNoHost() {
        // Stop the server instance (no hosts are required)
        $this->ccm->stop();

        // Create and close sessions asynchronously
        $connect_future = $this->cluster->connectAsync();
        try {
            $connect_future->get();
            $this->fail("Connection established");
        } catch (Exception\RuntimeException $re) {
            $this->assertTrue(true);
        }

        // Restart the server instance
        $this->ccm->start();

    }
}
