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
     * Default port for connection.
     * Default: 9042
     *
     * @var int
     */
    private $port;

    /**
     * @access private
     */
    public function __construct()
    {
        $this->contactPoints            = '127.0.0.1';
        $this->loadBalancingPolicy      = \Cassandra::LOAD_BALANCING_ROUND_ROBIN;
        $this->useTokenAwareRouting     = true;
        $this->defaultConsistency       = \Cassandra::CONSISTENCY_ONE;
        $this->defaultPageSize          = 10000;
        $this->defaultTimeout           = null;
        $this->port                     = 9042;
    }

    /**
     * Returns a Cluster Instance.
     *
     * @return \Cassandra\Cluster Cluster instance
     */
    public function build()
    {
        $options = new ExecutionOptions();
        $cluster = cassandra_cluster_new();
        $ssl     = null;

        $options->consistency       = $this->defaultConsistency;
        $options->pageSize          = $this->defaultPageSize;
        $options->timeout           = $this->defaultTimeout;

        switch ($this->loadBalancingPolicy) {
            case \Cassandra::LOAD_BALANCING_ROUND_ROBIN:
                cassandra_cluster_set_load_balance_round_robin($cluster);
                break;
            case \Cassandra::LOAD_BALANCING_DC_AWARE_ROUND_ROBIN:
                cassandra_cluster_set_load_balance_dc_aware($cluster, $this->localDatacenter, $this->hostPerRemoteDatacenter, $this->useRemoteDatacenterForLocalConsistencies);
                break;
        }

        cassandra_cluster_set_token_aware_routing($cluster, $this->useTokenAwareRouting);

        if (!is_null($this->username) && !is_null($this->password)) {
            cassandra_cluster_set_credentials($cluster, $this->username, $this->password);
        }

        if (!is_null($this->connectTimeout)) {
            cassandra_cluster_set_connect_timeout($cluster, $this->connectTimeout);
        }

        if (!is_null($this->requestTimeout)) {
            cassandra_cluster_set_request_timeout($cluster, $this->requestTimeout);
        }

        if (!is_null($this->sslOptions)) {
            $ssl = $this->sslOptions->resource();
            cassandra_cluster_set_ssl($cluster, $ssl);
        }

        cassandra_cluster_set_contact_points($cluster, $this->contactPoints);

        if (!is_null($this->port) && $this->port != 9042) {
            cassandra_cluster_set_port($cluster, $this->port);
        }

        return new DefaultCluster($cluster, $options, $ssl);
    }

    /**
     * Configures default consistency for all requests.
     *
     * @param int $consistency A consistency level, must be one of Cassandra::CONSISTENCY_* values
     *
     * @return self
     */
    public function withDefaultConsistency($consistency)
    {
        if (!in_array($consistency, array(
                \Cassandra::CONSISTENCY_ANY,
                \Cassandra::CONSISTENCY_ONE,
                \Cassandra::CONSISTENCY_TWO,
                \Cassandra::CONSISTENCY_THREE,
                \Cassandra::CONSISTENCY_QUORUM,
                \Cassandra::CONSISTENCY_ALL,
                \Cassandra::CONSISTENCY_LOCAL_QUORUM,
                \Cassandra::CONSISTENCY_EACH_QUORUM,
                \Cassandra::CONSISTENCY_SERIAL,
                \Cassandra::CONSISTENCY_LOCAL_SERIAL,
                \Cassandra::CONSISTENCY_LOCAL_ONE,
            ))) {
            throw new InvalidArgumentException(sprintf(
                'Invalid consistency, must be one of '.
                'Cassandra::CONSISTENCY_*, %s given',
                var_export($consistency, true)
            ));
        }

        $this->defaultConsistency = $consistency;

        return $this;
    }

    /**
     * Configures default page size for all results.
     * Set to -1 to disable paging altogether.
     *
     * @param int $pageSize default page size
     *
     * @return self
     */
    public function withDefaultPageSize($pageSize)
    {
        if (!(is_numeric($pageSize) && ($pageSize === -1 || $pageSize > 0))) {
            throw new InvalidArgumentException(sprintf(
                'Page size must be a positive integer or exactly -1, %s given',
                var_export($pageSize, true)
            ));
        }

        $this->defaultPageSize = $pageSize;

        return $this;
    }

    /**
     * Configures default timeout for future resolution in blocking operations
     * Set to null to disable (default).
     *
     * @param int|null $timeout timeout value
     *
     * @return self
     */
    public function withDefaultTimeout($timeout)
    {
        if (!(is_null($timeout) || is_numeric($timeout) && $timeout > 0)) {
            throw new InvalidArgumentException(sprintf(
                'Page size must be a positive integer or exactly -1, %s given',
                var_export($timeout, true)
            ));
        }

        $this->defaultTimeout = $timeout;

        return $this;
    }

    /**
     * Configures the initial endpoints. Note that the driver will
     * automatically discover and connect to the rest of the cluster.
     *
     * @param string $host an ip address string
     * @param string ...   additional addresses
     *
     * @return self
     */
    public function withContactPoints($host)
    {
        $this->contactPoints = implode(',', func_get_args());

        return $this;
    }

    /**
     * Configures the port for endpoints.
     *
     * @param int $port an ip address string
     *
     * @return self
     */
    public function withPort($port)
    {
        $this->port = $port;

        return $this;
    }

    /**
     * Configures this cluster to use a round robin load balancing policy.
     *
     * @return self
     */
    public function withRoundRobinLoadBalancingPolicy()
    {
        $this->loadBalancingPolicy = \Cassandra::LOAD_BALANCING_ROUND_ROBIN;

        return $this;
    }

    /**
     * Configures this cluster to use a datacenter aware round robin load balancing policy.
     *
     * @param string $localDatacenter                          Name of the local datacenter
     * @param int    $hostPerRemoteDatacenter                  Maximum number of hosts to try in remote datacenters
     * @param bool   $useRemoteDatacenterForLocalConsistencies Allow using hosts from remote datacenters to execute statements with local consistencies
     *
     * @return self
     */
    public function withDatacenterAwareRoundRobinLoadBalancingPolicy($localDatacenter, $hostPerRemoteDatacenter, $useRemoteDatacenterForLocalConsistencies)
    {
        $localDatacenter                          = (string) $localDatacenter;
        $hostPerRemoteDatacenter                  = (int) $hostPerRemoteDatacenter;
        $useRemoteDatacenterForLocalConsistencies = (bool) $useRemoteDatacenterForLocalConsistencies;

        if ($hostPerRemoteDatacenter < 0) {
            throw new InvalidArgumentException(sprintf('Number of hosts per remote datacenter cannot be negative, %s given', $hostPerRemoteDatacenter));
        }

        $this->loadBalancingPolicy                      = \Cassandra::LOAD_BALANCING_DC_AWARE_ROUND_ROBIN;
        $this->localDatacenter                          = $localDatacenter;
        $this->hostPerRemoteDatacenter                  = $hostPerRemoteDatacenter;
        $this->useRemoteDatacenterForLocalConsistencies = $useRemoteDatacenterForLocalConsistencies;

        return $this;
    }

    /**
     * Enable token aware routing.
     *
     * @param bool $enabled Whether to enable token aware routing (default: `true`)
     *
     * @return self
     */
    public function withTokenAwareRouting($enabled = true)
    {
        $this->useTokenAwareRouting = (bool) $enabled;

        return $this;
    }

    /**
     * Configures cassandra authentication.
     *
     * @param string $username Username
     * @param string $password Password
     *
     * @return self
     */
    public function withCredentials($username, $password)
    {
        $this->username = (string) $username;
        $this->password = (string) $password;

        return $this;
    }

    /**
     * Timeout used for establishing TCP connections.
     *
     * @param float $timeout Timeout value in seconds, can be fractional
     *
     * @return self
     */
    public function withConnectTimeout($timeout)
    {
        $this->connectTimeout = (float) $timeout;

        return $this;
    }

    /**
     * Timeout used for waiting for a response from a node.
     *
     * @param float $timeout Timeout value in seconds, can be fractional
     *
     * @return self
     */
    public function withRequestTimeout($timeout)
    {
        $this->requestTimeout = (float) $timeout;

        return $this;
    }

    /**
     * Set up ssl context.
     *
     * @param SSLOptions $options a preconfigured ssl context
     *
     * @return self
     */
    public function withSSL(SSLOptions $options)
    {
        $this->sslOptions = $options;

        return $this;
    }
}
