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

use Cassandra\Exception\ProtocolException;
use InvalidArgumentException;

class PagingIntegrationTest extends BasicIntegrationTest {
    protected function setUp(): void {
        parent::setUp();

        $this->session->execute("CREATE TABLE {$this->tableNamePrefix} (key int PRIMARY KEY, value int)");

        for ($i = 0; $i < 10; $i++) {
            $this->session->execute(
                "INSERT INTO {$this->tableNamePrefix} (key, value) VALUES (?, ?)",
                array("arguments" => array($i, $i))
            );
        }
    }

    /**
     * Convert a single column of a collection of rows into an array
     *
     * @param Cassandra\Rows
     * @param string Column name to consolidate
     *
     * @return array Array of column values using the provided column name
     */
    private static function convertRowsToArray($rows, $columnName) {
        $values = array();
        foreach ($rows as $row) {
            $values []= $row[$columnName];
        }
        return $values;
    }

    /**
     * Generate a random string
     *
     * @param int $length Length of string to generate (DEFAULT: random length
     *                    from 1 - 1024 characters)
     * @return string Randomly genreated text
     */
    private function randomString($length = -1) {
        // Determine if the length should be random
        if ($length < 0) {
            $length = mt_rand(1, 1024);
        }

        // Generate the random string from the below character set
        $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ';
        $charactersLength = strlen($characters);
        $randomString = '';
        foreach (range(1, $length) as $i) {
            $randomString .= $characters[rand(0, $charactersLength - 1)];
        }
        return $randomString;
    }

    /**
     * Page through the results while validating no memory leaks exists
     *
     * @param $start Starting memory value
     * @return int Number of rows visited
     */
    private function validatePageResults($rows) {
        // Get the starting memory usage
        $start = memory_get_usage() / 1024;
        if (Integration::isDebug() && Integration::isVerbose()) {
            fprintf(STDOUT, "Start Usage: %dkb" . PHP_EOL, $start);
        }

         // Page over each result set and count the number of rows visited
        $count = $rows->count();
        while ($rows = $rows->nextPage()) {
            if ($rows->count() != 0) {
                $count += $rows->count();
                if (Integration::isDebug() && Integration::isVerbose()) {
                    fprintf(STDOUT, "Page %d: Current memory usage is %dkb" . PHP_EOL,
                    ($count / 2), ((memory_get_usage() / 1024) - $start));
                }
            }
        }

        // Get the final memory usage (and apply a tolerance to compensate for GC)
        $end = memory_get_usage() / 1024;
        if (Integration::isDebug() && Integration::isVerbose()) {
            fprintf(STDOUT, "End Usage: %dkb [%dkb]" . PHP_EOL, $end, ($end - $start));
        }
        $difference = ($end - $start) - 20; // 20KB tolerance
        $this->assertLessThanOrEqual(0, $difference);

        // Return the number of rows visited
        return $count;
    }

    /**
     * Use paging state token
     *
     * This test ensure that the paging state token is properly returned and can be
     * used to get the next page results.
     *
     * @test
     * @ticket PHP-46
     */
    public function testPagingToken() {
        $results = array();

        for ($i = 0; $i < 10; $i++) {
            $options = array("page_size" => 1);
            if (isset($result)) {
                $options["paging_state_token"] = $result->pagingStateToken();
            }
            $result = $this->session->execute("SELECT * FROM {$this->tableNamePrefix}", $options);
            if(PHP_MAJOR_VERSION >= 8) {
                $this->assertCount(1, $result);
            }
            else {
                $this->assertEquals(1, count($result));
            }

            $row = $result->first();
            $results[] = $row["value"];
        }

        sort($results);
        $this->assertEquals($results, range(0, 9));
    }

    /**
     * Use an invalid paging state token
     *
     * This test driver properly handles Cassandra's error result when passing
     * an invalid paging state token.
     *
     * @test
     * @ticket PHP-46
     */
    public function testInvalidToken() {
        $this->expectException(ProtocolException::class);
        $this->expectExceptionMessage('Invalid value for the paging state');
        $this->session->execute(
            "SELECT * FROM {$this->tableNamePrefix}",
            array("paging_state_token" => "invalid")
        );
    }

    /**
     * Use a null paging token
     *
     * This test ensures that the driver properly handles a null paging state
     * token.
     *
     * @test
     * @ticket PHP-46
     */
    public function testNullToken() {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessageMatches('/paging_state_token must be a string.*/');
        $this->session->execute(
            "SELECT * FROM {$this->tableNamePrefix}",
            array("paging_state_token" => null)
        );
    }

    /**
     * Verify next page caching in `Cassandra\Rows`
     *
     * @test
     * @ticket PHP-101
     */
    public function testNextPageCaching() {
        $results = array();
        $pageSize = 2;

        // Get first page
        $rows = $this->session->execute(
            "SELECT * FROM {$this->tableNamePrefix}",
            array("page_size" => $pageSize)
        );
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $rows);
        }
        else {
            $this->assertEquals($rows->count(), $pageSize);
        }
        $values = self::convertRowsToArray($rows, "value");

        // Get next page (verify that it's a different page)
        $nextRows = $rows->nextPage();
        $nextValues = self::convertRowsToArray($nextRows, "value");
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $nextRows);
        }
        else {
            $this->assertEquals($nextRows->count(), $pageSize);
        }
        $this->assertNotEquals($values, $nextValues);

        // Get next page again (verify that it's the same)
        $nextRowsAgain = $rows->nextPage();
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $nextRowsAgain);
        }
        else {
            $this->assertEquals($nextRowsAgain->count(), $pageSize);
        }
        $nextValuesAgain = self::convertRowsToArray($nextRowsAgain, "value");
        $this->assertEquals($nextValues, $nextValuesAgain);

        // Get next page asynchonously (verify that it's the same)
        $nextRowsAsync = $rows->nextPageAsync()->get();
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $nextRowsAsync);
        }
        else {
            $this->assertEquals($nextRowsAsync->count(), $pageSize);
        }
        $nextValuesAsync = self::convertRowsToArray($nextRowsAsync, "value");
        $this->assertEquals($nextValues, $nextValuesAsync);

        // Get the next page's page (verify that it's a different page)
        $lastRows = $nextRows->nextPage();
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $lastRows);
        }
        else {
            $this->assertEquals($lastRows->count(), $pageSize);
        }
        $lastValues = self::convertRowsToArray($lastRows, "value");
        $this->assertNotEquals($nextValues, $lastValues);
    }

    /**
     * Verify next page asynchronous caching in `Cassandra\Rows`
     *
     * @test
     * @ticket PHP-101
     */
    public function testNextPageAsyncCaching() {
        $results = array();
        $pageSize = 2;

        // Get first page
        $rows = $this->session->execute(
            "SELECT * FROM {$this->tableNamePrefix}",
            array("page_size" => $pageSize)
        );
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $rows);
        }
        else {
            $this->assertEquals($rows->count(), $pageSize);
        }
        $values = self::convertRowsToArray($rows, "value");

        // Get next page asynchronously (verify that it's a different page)
        $nextRowsAsync = $rows->nextPageAsync()->get();
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $nextRowsAsync);
        }
        else {
            $this->assertEquals($nextRowsAsync->count(), $pageSize);
        }
        $nextValuesAsync = self::convertRowsToArray($nextRowsAsync, "value");
        $this->assertNotEquals($values, $nextValuesAsync);

        // Get next page asynchronously again (verify that it's the same)
        $nextRowsAgainAsync = $rows->nextPageAsync()->get();
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $nextRowsAgainAsync);
        }
        else {
            $this->assertEquals($nextRowsAgainAsync->count(), $pageSize);
        }
        $nextValuesAgainAsync = self::convertRowsToArray($nextRowsAgainAsync, "value");
        $this->assertEquals($nextValuesAsync, $nextValuesAgainAsync);

        // Get the next page again synchonously (verify that it's the same)
        $nextRows = $rows->nextPage();
        $nextValues = self::convertRowsToArray($nextRows, "value");
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $nextRows);
        }
        else {
            $this->assertEquals($nextRows->count(), $pageSize);
        }
        $this->assertEquals($nextValuesAsync, $nextValues);

        // Get the next page's page asynchronously (verify that it's a different page)
        $lastRowsAsync = $nextRowsAsync->nextPageAsync()->get();
        if(PHP_MAJOR_VERSION >= 8) {
            $this->assertCount($pageSize, $lastRowsAsync);
        }
        else {
            $this->assertEquals($lastRowsAsync->count(), $pageSize);
        }
        $lastValuesAsync = self::convertRowsToArray($lastRowsAsync, "value");
        $this->assertNotEquals($nextValuesAsync, $lastValuesAsync);
    }

    /**
     * Paging advancement does not create memory leak
     *
     * This test will ensure that the driver does not create memory leaks
     * associated advancing to the next page of results.
     *
     * @test
     * @ticket PHP-101
     */
    public function testNoPagingMemoryLeak() {
        // Create the user types and table for the test
        $this->session->execute("DROP TABLE {$this->tableNamePrefix}");
        $this->session->execute("CREATE TYPE price_history (time timestamp, price float)");
        $priceHistory = Type::userType(
            "time", Type::timestamp(),
            "price", Type::float());
        $this->session->execute("CREATE TYPE purchase_stats (day_of_week int, total_purchases int)");
        $purchaseStats = Type::userType(
            "day_of_week", Type::int(),
            "total_purchases", Type::int());
        $this->session->execute(
            "CREATE TABLE {$this->tableNamePrefix} " .
            "(id uuid PRIMARY KEY, history frozen<price_history>, stats frozen<purchase_stats>, " .
            " comments text)"
        );

        // Populate the table with some random data
        $totalInserts = 500;
        $statement = $this->session->prepare("INSERT INTO {$this->tableNamePrefix}
            (id, history, stats, comments) VALUES (?, ?, ?, ?)");
        foreach (range(1, $totalInserts) as $i) {
            // Create the values for the insert
            $history = $priceHistory->create(
                "time", new Timestamp(mt_rand(1270094400000, 1459483200000)), // 04-01-2010 - 04-01-2016
                "price", new Float((mt_rand(1, 1000) / 100))
            );
            $stats = $purchaseStats->create(
                "day_of_week", mt_rand(0, 6),
                "total_purchases", mt_rand(0, 1000)
            );
            $values = array(
                new Uuid(),
                $history,
                $stats,
                $this->randomString()
            );

            $this->session->execute($statement, array("arguments" => $values));
        }

        // Select all the rows in the table using paging
        $rows = $this->session->execute("SELECT * FROM {$this->tableNamePrefix}", array("page_size" => 2));

        // Validate paging and ensure all the rows were read
        $count = $this->validatePageResults($rows);
        $this->assertEquals($totalInserts, $count);
    }
}
