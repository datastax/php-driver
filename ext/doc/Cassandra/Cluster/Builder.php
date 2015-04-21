<?php

/**
 * Copyright 2015 DataStax, Inc.
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

namespace Cassandra\Cluster;

use Cassandra\SSLOptions;
use Cassandra\DefaultCluster;
use Cassandra\ExecutionOptions;
use Cassandra\Exception\InvalidArgumentException;

/**
 * Cluster builder allows fluent configuration of the cluster instance.
 *
 * @see Cassandra::cluster()
 */
final class Builder
{
    /**
     * Contact points, defaults to "127.0.0.1".
     *
     * @var string
     */
    private $contactPoints;

    /**
     * Either Cassandra::LOAD_BALANCING_ROUND_ROBIN or Cassandra::LOAD_BALANCING_DC_AWARE_ROUND_ROBIN.
     *
     * @var int
     */
    private $loadBalancingPolicy;

    /**
     * Name of the local datacenter.
     *
     * @var string
     */
    private $localDatacenter;

    /**
     * Maximum number of hosts to try in remote datacenters.
     *
     * @var int
     */
    private $hostPerRemoteDatacenter;

    /**
     * Allow using hosts from remote datacenters to execute statements with local consistencies.
     *
     * @var bool
     */
    private $useRemoteDatacenterForLocalConsistencies;

    /**
     * Enable Token-aware routing.
     *
     * @var bool
     */
    private $useTokenAwareRouting;

    /**
     * Username for authentication.
     *
     * @var string
     */
    private $username;

    /**
     * Password for authentication.
     *
     * @var string
     */
    private $password;

    /**
     * Connection timeout.
     *
     * @var float
     */
    private $connectTimeout;

    /**
     * Request timeout.
     *
     * @var float
     */
    private $requestTimeout;

    /**
     * sslOptions.
     *
     * @var SSLOptions
     */
    private $sslOptions;

    /**
     * Default consistency for requests
     * One of Cassandra::CONSISTENCY_*
     * Default: Cassandra::CONSISTENCY_ONE.
     *
     * @var int
     */
    private $defaultConsistency;

    /**
     * Default results page size
     * Default: 10000.
     *
     * @var int
     */
    private $defaultPageSize;

    /**
     * Default timeout for future resolution in blocking operations
     * Default: null.
     *
     * @var float
     */
    private $defaultTimeout;

    /**
     * Returns a Cluster Instance.
     *
     * @return \Cassandra\Cluster Cluster instance
     */
    public function build() {}

    /**
     * Configures default consistency for all requests.
     *
     * @param int $consistency A consistency level, must be one of Cassandra::CONSISTENCY_* values
     *
     * @return self
     */
    public function withDefaultConsistency($consistency) {}

    /**
     * Configures default page size for all results.
     * Set to `null` to disable paging altogether.
     *
     * @param int|null $pageSize default page size
     *
     * @return self
     */
    public function withDefaultPageSize($pageSize) {}

    /**
     * Configures default timeout for future resolution in blocking operations
     * Set to null to disable (default).
     *
     * @param int|null $timeout timeout value
     *
     * @return self
     */
    public function withDefaultTimeout($timeout) {}

    /**
     * Configures the initial endpoints. Note that the driver will
     * automatically discover and connect to the rest of the cluster.
     *
     * @param string $host an ip address string
     * @param string ...   additional addresses
     *
     * @return self
     */
    public function withContactPoints($host) {}

    /**
     * Configures this cluster to use a round robin load balancing policy.
     *
     * @return self
     */
    public function withRoundRobinLoadBalancingPolicy() {}

    /**
     * Configures this cluster to use a datacenter aware round robin load balancing policy.
     *
     * @param string $localDatacenter                          Name of the local datacenter
     * @param int    $hostPerRemoteDatacenter                  Maximum number of hosts to try in remote datacenters
     * @param bool   $useRemoteDatacenterForLocalConsistencies Allow using hosts from remote datacenters to execute statements with local consistencies
     *
     * @return self
     */
    public function withDatacenterAwareRoundRobinLoadBalancingPolicy($localDatacenter, $hostPerRemoteDatacenter, $useRemoteDatacenterForLocalConsistencies) {}

    /**
     * Enable token aware routing.
     *
     * @param bool $enabled Whether to enable token aware routing (default: `true`)
     *
     * @return self
     */
    public function withTokenAwareRouting($enabled = true) {}

    /**
     * Configures cassandra authentication.
     *
     * @param string $username Username
     * @param string $password Password
     *
     * @return self
     */
    public function withCredentials($username, $password) {}

    /**
     * Timeout used for establishing TCP connections.
     *
     * @param float $timeout Timeout value in seconds, can be fractional
     *
     * @return self
     */
    public function withConnectTimeout($timeout) {}
    /**
     * Timeout used for waiting for a response from a node.
     *
     * @param float $timeout Timeout value in seconds, can be fractional
     *
     * @return self
     */
    public function withRequestTimeout($timeout) {}

    /**
     * Set up ssl context.
     *
     * @param SSLOptions $options a preconfigured ssl context
     *
     * @return self
     */
    public function withSSL(SSLOptions $options) {}

    /**
     * Enable persistent sessions and clusters
     *
     * @param bool $enabled whether to enable persistent sessions and clusters.
     *
     * @return self
     */
    public function withPersistentSessions($enabled = true) {}
}
