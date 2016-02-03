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
     * Name of the suite/test to listen for when finalizing test suite
     * teardown.
     *
     * @var string
     */
    private static $endingSuiteTestName;

    /**
     * Determine if the debug argument was used when starting PHPUnit.
     *
     * @return bool True if debug argument was used; false otherwise
     */
    private function isDebug() {
        return in_array("--debug", $_SERVER['argv']);
    }

    /**
     * Determine if test suite is an integration test suite.
     *
     * @param PHPUnit_Framework_TestSuite $testSuite Test suite
     * @param bool True if suite/test is starting; false otherwise
     * @return bool True if integration suite/test; false otherwise.
     */
    private function isIntegrationSuiteOrTest(PHPUnit_Framework_TestSuite $testSuite, $isStarting) {
        // Determine if the suite/test name should be initialized
        if ($isStarting) {
            // Determine if the suite/test is the setup suite/test
            if (self::$endingSuiteTestName == "") {
                // Determine if this is a test suite or integration test
                $name = $testSuite->getName();
                if (strpos($name, "integration") !== false) {
                    // Test suite
                    self::$endingSuiteTestName = $name;
                    return true;
                } else if (strpos(strtolower($name), "integration") !== false ||
                    is_file($name = end($_SERVER['argv']))) {
                    if (strpos(strtolower($name), "integration") !== false) {
                        // Integration test
                        self::$endingSuiteTestName = $name;
                        return true;
                    }
                }
            }
        } else {
            // Determine if the suite/test is the teardown suite/test
            if (self::$endingSuiteTestName != "") {
                if ($testSuite->getName() == self::$endingSuiteTestName ||
                    end($_SERVER['argv']) == self::$endingSuiteTestName) {
                    // Teardown suite/test
                    self::$endingSuiteTestName = "";
                    return true;
                }
            }
        }

        /*
         * Unable to determine if suite/test is for integration purposes or the
         * suite/test has already been initialized
         */
        return false;
    }

    /**
     * Destroy all CCM clusters when starting and stopping the integration tests.
     * This will only be run on startup and shutdown.
     */
    private function removeTestClusters() {
        $ccm = new \CCM(CCM::DEFAULT_CASSANDRA_VERSION, true);
        $ccm->removeAllClusters();
    }

    /**
     * Handle the overall teardown steps for a test suite
     *
     * @param PHPUnit_Framework_TestSuite $testSuite Test suite ending
     */
    public function endTestSuite(PHPUnit_Framework_TestSuite $testSuite) {
        // Determine type of test suite being ending
        if ($this->isIntegrationSuiteOrTest($testSuite, false)) {
            if ($this->isDebug()) {
                echo PHP_EOL . PHP_EOL . "--=== Entering PHP Driver Integration Tests Teardown ===--" . PHP_EOL;
            }
            $this->removeTestClusters();
        }
    }

    /**
     * Handle the overall setup steps for a test suite
     *
     * @param PHPUnit_Framework_TestSuite $testSuite Test suite starting
     */
    public function startTestSuite(PHPUnit_Framework_TestSuite $testSuite) {
        // Determine type of test suite being started
        if ($this->isIntegrationSuiteOrTest($testSuite, true)) {
            if ($this->isDebug()) {
                echo PHP_EOL . "--=== Entering PHP Driver Integration Tests Setup ===--" . PHP_EOL;
            }
            $this->removeTestClusters();
        }
    }
}