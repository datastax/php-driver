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

class PagingIntegrationTest extends BasicIntegrationTest
{
    public function setUp() {
        parent::setUp();

        $statement = new SimpleStatement(
            "CREATE TABLE {$this->tableNamePrefix} (key int PRIMARY KEY, value int)"
        );
        $this->session->execute($statement);

        $statement = new SimpleStatement(
            "INSERT INTO {$this->tableNamePrefix} (key, value) VALUES (?, ?)"
        );

        for ($i = 0; $i < 10; $i++) {
            $options = new ExecutionOptions(array(
                "arguments" => array($i, $i)
            ));
            $this->session->execute($statement, $options);
        }
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

        $statement = new SimpleStatement(
            "SELECT * FROM {$this->tableNamePrefix}"
        );

        for ($i = 0; $i < 10; $i++) {
            $options = array("page_size" => 1);
            if (isset($result)) {
                $options["paging_state_token"] = $result->pagingStateToken();
            }
            $result = $this->session->execute($statement, new ExecutionOptions($options));
            $this->assertEquals(1, count($result));

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
     *
     * @expectedException Cassandra\Exception\ProtocolException
     * @expectedExceptionMessage Invalid value for the paging state
     */
    public function testInvalidToken() {
        $statement = new SimpleStatement(
            "SELECT * FROM {$this->tableNamePrefix}"
        );

        $options = new ExecutionOptions(array(
            "paging_state_token" => "invalid"
        ));

        $result = $this->session->execute($statement, $options);
    }

    /**
     * Use a null paging token
     *
     * This test ensures that the driver properly handles a null paging state
     * token.
     *
     * @test
     * @ticket PHP-46
     *
     * @expectedException Cassandra\Exception\InvalidArgumentException
     * @expectedExceptionMessageRegExp |paging_state_token must be a string.*|
     */
    public function testNullToken() {
        $statement = new SimpleStatement(
            "SELECT * FROM {$this->tableNamePrefix}"
        );

        $options = new ExecutionOptions(array(
            "paging_state_token" => null
        ));

        $result = $this->session->execute($statement, $options);
    }
}
