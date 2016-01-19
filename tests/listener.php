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

/**
 * Listener class to allow for global setup/teardown fixture when executing
 * test suites. This will allow for different bootstrap mechanisms for the
 * varying test suites.
 */
class TestSuiteListener extends \PHPUnit_Framework_BaseTestListener {
    /**
     * Destroy all CCM clusters when starting and stopping the integration tests.
     * This will only be run on startup and shutdown.
     */
    private static function remove_test_clusters() {
        $ccm = new CCM();
        $ccm->removeAllClusters();
    }

    /**
     * Handle the overall teardown steps for a test suite
     *
     * @param PHPUnit_Framework_TestSuite $test_suite Test suite ending
     */
    public function endTestSuite(PHPUnit_Framework_TestSuite $test_suite) {
        // Determine type of test suite being ending
        if (strpos($test_suite->getName(), "integration") !== false ) {
            printf("\nEntering PHP Driver Integration Tests Teardown\n");
            TestSuiteListener::remove_test_clusters();
        }
    }

    /**
     * Handle the overall setup steps for a test suite
     *
     * @param PHPUnit_Framework_TestSuite $test_suite Test suite starting
     */
    public function startTestSuite(PHPUnit_Framework_TestSuite $test_suite) {
        // Determine type of test suite being started
        if (strpos($test_suite->getName(), "integration") !== false ) {
            printf("\nEntering PHP Driver Integration Tests Setup\n");
            TestSuiteListener::remove_test_clusters();
        }
    }
}