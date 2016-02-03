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
 * Retry policy integration tests.
 */
class RetryPolicyIntegrationTest extends BasicIntegrationTest {
    /**
     * Insert query generated for a retry policy test.
     *
     * @var string
     */
    private $insertQuery;

    /**
     * Setup the retry policy for multiple nodes.
     */
    public function setUp() {
        // Ensure there are three nodes in data center one with RF = 3
        $this->numberDC1Nodes = 3;
        $this->replicationFactor = 3;

        // Process parent setup steps
        parent::setUp();

        // Create the table
        $query = "CREATE TABLE {$this->tableNamePrefix} (key int, value_int int, PRIMARY KEY(key, value_int))";
        $this->session->execute(new SimpleStatement($query));

        // Create the insert query
        $this->insertQuery = "INSERT INTO {$this->tableNamePrefix} (key, value_int) VALUES (?, ?)";
    }

    /**
     * Teardown; Ensure all nodes have been restarted
     */
    public function tearDown() {
        $nodes = range(1, $this->numberDC1Nodes);
        $this->ccm->resumeNode($nodes);

        // Process parent teardown steps
        parent::tearDown();
    }

    /**
     * Insert n values into the table for a given key.
     *
     * @param RetryPolicy $policy RetryPolicy to use when executing statements
     * @param $key Key value
     * @param $numberOfInserts Number of inserts to perform
     * @param $consistency Consistency level to execute statement
     */
    private function insert(RetryPolicy $policy, $key, $numberOfInserts, $consistency) {
        try {
            // Create and prepare the insert statements
            $prepare = $this->session->prepare($this->insertQuery);
            $simple = new SimpleStatement($this->insertQuery);
            $batch = new BatchStatement(\Cassandra::BATCH_UNLOGGED);

            // Create the batched inserts
            foreach (range(1, $numberOfInserts) as $i) {
                $values = array(
                    $key,
                    $i
                );
                if ($i % 2 == 0) {
                    $batch->add($prepare, $values);
                } else {
                    $batch->add($simple, $values);
                }
            }

            // Execute the batched insert
            $options = new ExecutionOptions(array(
                "consistency" => $consistency,
                "retry_policy" => $policy
            ));
            $this->session->execute($batch, $options);
        } catch (Exception\TimeoutException $te) {
            $this->insert($policy, $key, $numberOfInserts, $consistency);
        }
    }

    /**
     * Assert n values in the table for a given key.
     *
     * @param RetryPolicy $policy RetryPolicy to use when executing statements
     * @param $key Key value
     * @param $numberOfAsserts Number of inserts to perform
     * @param $consistency Consistency level to execute statement
     */
    private function assert(RetryPolicy $policy, $key, $numberOfAsserts, $consistency) {
        try {
            // Select the values
            $query = "SELECT value_int FROM {$this->tableNamePrefix} WHERE key = {$key}";
            $statement = new SimpleStatement($query);
            $options = new ExecutionOptions(array(
                "consistency" => $consistency,
                "retry_policy" => $policy
            ));
            $rows = $this->session->execute($statement, $options);

            // Assert the values
            $this->assertCount($numberOfAsserts, $rows);
            foreach ($rows as $i => $row) {
                $this->assertEquals(($i + 1), $row["value_int"]);
            }
        } catch (Exception\TimeoutException $te) {
            $this->insert($policy, $key, $numberOfAsserts, $consistency);
        }
    }

    /**
     * Statement execution supports downgrading consistency retry policy.
     *
     * This test will ensure that the PHP driver supports the downgrading
     * retry policy when executing statements.
     *
     * @test
     * @ticket PHP-60
     *
     * @cassandra-version-2.0
     */
    public function testDowngradingPolicy() {
        // Create the retry policy
        $policy = new RetryPolicy\DowngradingConsistency();

        // Disable node one
        $this->ccm->pauseNode(1);

        // Insert and assert values with CONSISTENCY_ALL
        $this->insert($policy, 0, 25, \Cassandra::CONSISTENCY_ALL);
        $this->assert($policy, 0, 25, \Cassandra::CONSISTENCY_ALL);

        // Disable node three
        $this->ccm->pauseNode(3);

        // Insert and assert values with CONSISTENCY_ALL
        $this->insert($policy, 2, 25, \Cassandra::CONSISTENCY_ALL);
        $this->assert($policy, 2, 25, \Cassandra::CONSISTENCY_ALL);

        // Insert and assert values with CONSISTENCY_QUORUM
        $this->insert($policy, 3, 25, \Cassandra::CONSISTENCY_QUORUM);
        $this->assert($policy, 3, 25, \Cassandra::CONSISTENCY_QUORUM);

        // Insert and assert values with CONSISTENCY_TWO
        $this->insert($policy, 4, 25, \Cassandra::CONSISTENCY_TWO);
        $this->assert($policy, 4, 25, \Cassandra::CONSISTENCY_TWO);
    }

    /**
     * Statement execution supports fallthrough retry policy (write exception).
     *
     * This test will ensure that the PHP driver supports the ability to
     * provide any exception that occurs  when executing statements. This test
     * will ensure that a WriteTimeoutException occurs when a consistency level
     * cannot be achieved.
     *
     * @test
     * @ticket PHP-60
     *
     * @cassandra-version-2.0
     *
     * @expectedException \Cassandra\Exception\WriteTimeoutException
     * @expectedExceptionMessageRegExp Operation timed out - received only .*
     *                                 responses
     */
    public function testFallThroughPolicyWrite() {
        // Create the retry policy
        $policy = new RetryPolicy\Fallthrough();

        // Create a WriteTimeoutException
        $this->ccm->pauseNode(1);
        $this->insert($policy, 0, 25, \Cassandra::CONSISTENCY_ALL);
    }

    /**
     * Statement execution supports fallthrough retry policy (read exception).
     *
     * This test will ensure that the PHP driver supports the ability to
     * provide any exception that occurs  when executing statements. This test
     * will ensure that a ReadTimeoutException occurs when a consistency level
     * cannot be achieved.
     *
     * @test
     * @ticket PHP-60
     *
     * @cassandra-version-2.0
     *
     * @expectedException \Cassandra\Exception\ReadTimeoutException
     * @expectedExceptionMessageRegExp Operation timed out - received only .*
     *                                 responses
     */
    public function testFallThroughPolicyRead() {
        // Create the retry policy
        $policy = new RetryPolicy\Fallthrough();

        // Insert values with CONSISTENCY_ALL
        $this->insert($policy, 0, 25, \Cassandra::CONSISTENCY_ALL);

        // Create a ReadTimeoutException
        $this->ccm->pauseNode(1);
        $this->assert($policy, 0, 25, \Cassandra::CONSISTENCY_ALL);
    }
}