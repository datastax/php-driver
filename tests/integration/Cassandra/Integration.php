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
 * Base class to provide common integration test functionality.
 */
class Integration {
    //TODO: Remove these constant and make them configurable
    const IP_ADDRESS = "127.0.0.1";
    const CASSANDRA_VERSION = "2.2.4"; // This is needed for Windows testing right now
    const IS_CCM_SILENT = true;

    /**
     * Maximum length for the keyspace (server limit)
     */
    const KEYSPACE_MAXIMUM_LENGTH = 48;
    /**
     * Generic/Simple keyspace format
     */
    const SIMPLE_KEYSPACE_FORMAT = "CREATE KEYSPACE %s WITH replication = { 'class': %s };";
    /**
     * CQL query to retrieve the server (Cassandra/DSE) version
     */
    const SELECT_SERVER_VERSION = "SELECT release_version FROM system.local";

    /**
     * Static instance for connect and disconnect methods to utilize.
     *
     * @var Integration
     */
    private static $instance;
    /**
     * Generated keyspace name for the integration test.
     *
     * @var string
     */
    private $keyspaceName;
    /**
     * Handle for interacting with CCM.
     *
     * @var CCM
     */
    private $ccm;
    /**
     * Cluster instance.
     *
     * @var \Cassandra\Cluster
     */
    private $cluster;
    /**
     * Connected database session.
     *
     * @var \Cassandra\Session
     */
    private $session;
    /**
     * Version of Cassandra/DSE the session is connected to.
     *
     * @var string
     */
    private $serverVersion;

    /**
     * Create the integration helper instance.
     *
     * @param $className Name of the class for the executed test.
     * @param string $testName Name of the test being executed.
     * @param int $numberDC1Nodes Number of nodes in data center one
     *                            (DEFAULT: 1).
     * @param int $numberDC2Nodes Number of nodes in data center two
     *                            (DEFAULT: 0).
     * @param bool $isClientAuthentication True if client authentication
     *                                     should be enabled; false
     *                                     otherwise (DEFAULT: false).
     * @param bool $isSSL True if SSL should be enabled; false otherwise
     *                    (DEFAULT: false).
     * @return Integration Instance of the Integration class created.
     */
    public function __construct($className,
                                $testName = "",
                                $numberDC1Nodes = 1,
                                $numberDC2Nodes = 0,
                                $isClientAuthentication = false,
                                $isSSL = false) {
        // Generate the keyspace name for the test
        $this->keyspaceName = $this->getShortName($className);
        if (!empty($testName)) {
            $this->keyspaceName = $this->keyspaceName . "_" . $testName;
        }
        // Make all strings lowercase for case insensitive O/S (e.g. Windows)
        $this->keyspaceName = strtolower($this->keyspaceName);

        //Ensure the keyspace does not contain more to many characters
        if (strlen($this->keyspaceName) > self::KEYSPACE_MAXIMUM_LENGTH) {
            // Update the keyspace name with a unique ID
            $uniqueID = uniqid();
            $this->keyspaceName = substr($this->keyspaceName,
                0, self::KEYSPACE_MAXIMUM_LENGTH - strlen($uniqueID)) .
                $uniqueID;
        }

        // Create the Cassandra cluster for the test
        //TODO: Need to add the ability to switch the Cassandra version (command line)
        $this->ccm = new \CCM(self::CASSANDRA_VERSION, self::IS_CCM_SILENT);
        $this->ccm->setup($numberDC1Nodes, $numberDC2Nodes);
        if ($isClientAuthentication) {
            $this->ccm->setupClientVerification();
        }
        if ($isSSL) {
            $this->ccm->setupSSL();
        }
        $this->ccm->start();

        // Determine replication strategy and generate the query
        $replicationStrategy = "'SimpleStrategy', 'replication_factor': ";
        if ($numberDC2Nodes > 0) {
            $replicationStrategy = "'NetworkTopologyStrategy', 'dc1': " . $numberDC1Nodes . ", " .
                "'dc2': " . $numberDC2Nodes;
        } else {
            $replicationFactor = ($numberDC1Nodes % 2 == 0) ? $numberDC1Nodes / 2 : ($numberDC1Nodes + 1) / 2;
            $replicationStrategy .= $replicationFactor;
        }
        $query = sprintf(Integration::SIMPLE_KEYSPACE_FORMAT, $this->keyspaceName, $replicationStrategy);

        // Create the session and keyspace for the integration test
        try {
            // Create the session and integration test keypspace
            $this->cluster = \Cassandra::cluster()
                ->withContactPoints(Integration::IP_ADDRESS)
                ->build();
            $this->session = $this->cluster->connect();
            $statement = new SimpleStatement($query);
            $this->session->execute($statement);

            // Update the session to use the new keyspace by default
            $statement = new SimpleStatement("USE " . $this->keyspaceName);
            $this->session->execute($statement);

            // Get the server version the session is connected to
            $statement = new SimpleStatement(self::SELECT_SERVER_VERSION);
            $rows = $this->session->execute($statement);
            $this->serverVersion = $rows->first()["release_version"];

        } catch (Exception $e) {
            printf("Error Creating CCM Cluster: %s" . PHP_EOL . "%s" . PHP_EOL,
                $e->getMessage(), $e->getTraceAsString());
        }
    }

    public function __destruct() {
        // Drop keyspace for integration test (may or may have not been created)
        if (!is_null($this->session)) {
            try {
                $statement = new SimpleStatement('DROP KEYSPACE "' . $this->keyspaceName . '"');
                $this->session->execute($statement);
            } catch (Exception $e) {
                ; // no-op
            }
        }
    }

    public function __get($property) {
        if (property_exists($this, $property)) {
            return $this->$property;
        }
    }

    /**
     * Connect and establish basis for integration test.
     *
     * @param $className Name of the class for the executed test.
     * @param string $testName Name of the test being executed.
     * @param int $numberDC1Nodes Number of nodes in data center one
     *                            (DEFAULT: 1).
     * @param int $numberDC2Nodes Number of nodes in data center two
     *                            (DEFAULT: 0).
     * @param bool $isClientAuthentication True if client authentication
     *                                     should be enabled; false
     *                                     otherwise (DEFAULT: false).
     * @param bool $isSSL True if SSL should be enabled; false otherwise
     *                    (DEFAULT: false).
     * @return Integration Instance of the Integration class created.
     */
    public static function connect($className,
                                   $testName = "",
                                   $numberDC1Nodes = 1,
                                   $numberDC2Nodes = 0,
                                   $isClientAuthentication = false,
                                   $isSSL = false) {
        self::$instance = new Integration($className, $testName,
            $numberDC1Nodes, $numberDC2Nodes,
            $isClientAuthentication, $isSSL);
        return self::$instance;
    }

    /**
     * Disconnect and perform cleanup of the integration test.
     */
    public static function disconnect() {
        unset(self::$instance);
    }

    /**
     * Get the short name of the class without the namespacing.
     *
     * @param $className Class name to remove namespace from
     * @return string Short name for the class name
     */
    private function getShortName($className) {
        $function = new \ReflectionClass($className);
        return $function->getShortName();
    }
}