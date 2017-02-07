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
 * Future integration tests
 */
class FutureIntegrationTest extends BasicIntegrationTest {
    /**
     * Connect and close a session asynchronously
     *
     * @param int $repeat (Optional) Number of times to repeat the process
     *                               (default: 100)
     */
    private function connect_close_async($repeat = 100) {
        if (Integration::isDebug() && Integration::isVerbose()) {
            $start = memory_get_usage() / 1024;
        }
        for ($i = 0; $i < $repeat; ++$i) {
            //Create and close a session asynchronously (freeing PHP memory)
            $connect_future = $this->cluster->connectAsync();
            try {
                $session = $connect_future->get();
                $close_future = $session->closeAsync();
                $close_future->get();
            } catch (\Exception $e) {
                ;; // no-op
            }

            // Determine if memory stats should be displayed
            if (Integration::isDebug() && Integration::isVerbose()) {
                if ($i % 10 == 0) {
                    fprintf(STDOUT, "Current memory usage is %dkb" . PHP_EOL,
                        ((memory_get_usage() / 1024) - $start));
                }
            }
        }
    }

    /**
     * Ensure memory is freed when using async connection operations
     *
     * This test will ensure that when connecting to a server instance using
     * asynchronous connections, the memory associated with the future is
     * properly freed.
     *
     * @jira_ticket PHP-143
     * @test_category connection
     * @since 1.3.0
     * @excpected_result Driver will not leak memory
     */
    public function testNoFutureSessionMemoryLeak() {
        // Get the starting memory size
        $start = memory_get_usage() / 1024;
        if (Integration::isDebug() && Integration::isVerbose()) {
            fprintf(STDOUT, "Start Usage: %dkb" . PHP_EOL, $start);
        }

        // Create and close sessions asynchronously
        $this->connect_close_async();

        // Ensure all memory utilized by the futures were properly freed
        $end = memory_get_usage() / 1024;
        $difference = (int) ($end - $start);
        if (Integration::isDebug() && Integration::isVerbose()) {
            fprintf(STDOUT, "End Usage: %dkb [%dkb]" . PHP_EOL, $end, $difference);
        }
        $this->assertLessThanOrEqual(0, $difference - 2);  // 2kb tolerance
    }

    /**
     * Ensure memory is freed when using async connection operations during
     * failed connection to server
     *
     * This test will ensure that when connecting to a server instance using
     * asynchronous connections, the memory associated with the future is
     * properly freed when the connection could not be established.
     *
     * @jira_ticket PHP-143
     * @test_category connection
     * @since 1.3.0
     * @excpected_result Driver will not leak memory
     */
    public function testNoFutureSessionMemoryLeakWithNoHost() {
        // Stop the server instance (no hosts are required)
        $this->ccm->stop();

        // Get the starting memory size
        $start = memory_get_usage() / 1024;
        if (Integration::isDebug() && Integration::isVerbose()) {
            fprintf(STDOUT, "Start Usage: %dkb" . PHP_EOL, $start);
        }

        // Create and close sessions asynchronously
        $this->connect_close_async();

        // Ensure all memory utilized by the futures were properly freed
        $end = memory_get_usage() / 1024;
        $difference = (int) ($end - $start);
        if (Integration::isDebug() && Integration::isVerbose()) {
            fprintf(STDOUT, "End Usage: %dkb [%dkb]" . PHP_EOL, $end, $difference);
        }
        $this->assertLessThanOrEqual(0, $difference - 2);  // 2kb tolerance

        // Restart the server instance
        $this->ccm->start();

    }
}
