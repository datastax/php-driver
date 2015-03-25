<?php

namespace Cassandra\Cluster;

use Cassandra\DefaultCluster;
use Cassandra\ExecutionOptions;
use Cassandra\Exception\InvalidArgumentException;
use Cassandra\Util;

final class Builder
{
    const LOAD_BALANCING_ROUND_ROBIN          = 0;
    const LOAD_BALANCING_DC_AWARE_ROUND_ROBIN = 1;

    /**
     * Contact points, defaults to "127.0.0.1"
     * @var contactPoints
     */
    private $contactPoints;

    /**
     * Either Builder::LOAD_BALANCING_ROUND_ROBIN or Builder::LOAD_BALANCING_DC_AWARE_ROUND_ROBIN
     * @var integer
     */
    private $loadBalancingPolicy;

    /**
     * Name of the local datacenter
     * @var string
     */
    private $localDatacenter;

    /**
     * Maximum number of hosts to try in remote datacenters
     * @var integer
     */
    private $hostPerRemoteDatacenter;

    /**
     * Allow using hosts from remote datacenters to execute statements with local consistencies
     * @var boolean
     */
    private $useRemoteDatacenterForLocalConsistencies;

    /**
     * Enable Token-aware routing
     * @var boolean
     */
    private $useTokenAwareRouting;

    /**
     * Username for authentication
     * @var string
     */
    private $username;

    /**
     * Password for authentication
     * @var string
     */
    private $password;

    /**
     * Connection timeout
     * @var float
     */
    private $connectTimeout;

    /**
     * Request timeout
     * @var float
     */
    private $requestTimeout;

    /**
     * SSLContext
     * @var Cassandra\Cluster\SSLContext
     */
    private $sslContext;

    /**
     * Default consistency for requests
     * One of Cassandra::CONSISTENCY_*
     * Default: Cassandra::CONSISTENCY_ONE
     * @var int
     */
    private $defaultConsistency;

    /**
     * Default results page size
     * Default: 10000
     * @var int
     */
    private $defaultPageSize;

    /**
     * Default timeout for future resolution in blocking operations
     * Default: null
     * @var float
     */
    private $defaultTimeout;

    /**
     * @access private
     */
    public function __construct()
    {
        $this->contactPoints            = "127.0.0.1";
        $this->loadBalancingPolicy      = self::LOAD_BALANCING_ROUND_ROBIN;
        $this->useTokenAwareRouting     = true;
        $this->defaultConsistency       = \Cassandra::CONSISTENCY_ONE;
        $this->defaultPageSize          = 10000;
        $this->defaultTimeout           = null;
    }

    /**
     * Returns a Cluster Instance
     *
     * @return Cassandra\Cluster Cluster instance
     */
    public function build()
    {

        $options = new ExecutionOptions();
        $cluster = cassandra_cluster_new();

        $options->consistency       = $this->defaultConsistency;
        $options->pageSize          = $this->defaultPageSize;
        $options->timeout           = $this->defaultTimeout;

        switch($this->loadBalancingPolicy) {
            case self::LOAD_BALANCING_ROUND_ROBIN:
                cassandra_cluster_set_load_balance_round_robin($cluster);
                break;
            case self::LOAD_BALANCING_DC_AWARE_ROUND_ROBIN:
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

        if ($this->sslContext instanceof DefaultSSLContext) {
            cassandra_cluster_set_ssl($cluster, $context->resource());
        }

        cassandra_cluster_set_contact_points($cluster, $this->contactPoints);

        return new DefaultCluster($cluster, $options);
    }

    /**
     * Configures default consistency for all requests
     * @param int $consistency A consistency level, must be one of Cassandra::CONSISTENCY_* values
     * @return Cassandra\Cluster\Builder self
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
                \Cassandra::CONSISTENCY_LOCAL_ONE))) {
            throw new InvalidArgumentException(sprintf(
                "Invalid consistency, must be one of " .
                "Cassandra::CONSISTENCY_*, %s given",
                var_export($consistency, true)
            ));
        }

        $this->defaultConsistency = $consistency;
        return $this;
    }

    /**
     * Configures default page size for all results.
     * Set to -1 to disable paging altogether.
     * @param int $pageSize default page size
     */
    public function withDefaultPageSize($pageSize)
    {
      if (!(is_numeric($pageSize) && ($pageSize === -1 || $pageSize > 0))) {
          throw new InvalidArgumentException(sprintf(
              "Page size must be a positive integer or exactly -1, %s given",
              var_export($pageSize, true)
          ));
      }

      $this->defaultPageSize = $pageSize;
    }

    /**
     * Configures default timeout for future resolution in blocking operations
     * Set to null to disable (default)
     * @param int|null $timeout timeout value
     */
    public function withDefaultTimeout($timeout)
    {
      if (!(is_null($timeout) || is_numeric($timeout) && $timeout > 0)) {
          throw new InvalidArgumentException(sprintf(
              "Page size must be a positive integer or exactly -1, %s given",
              var_export($timeout, true)
          ));
      }

      $this->defaultTimeout = $timeout;
    }

    /**
     * Configures the initial endpoints. Note that the driver will automatically discover and connect to the rest of the cluster
     *
     * @param array $hosts an array of stings of ip addresses
     * @return Cassandra\Cluster\Builder self
     */
    public function withContactPoints(array $hosts)
    {
        $this->contactPoints = implode(',', $hosts);
        return $this;
    }

    /**
     * Configures this cluster to use a round robin load balancing policy.
     *
     * @return Cassandra\Cluster\Builder self
     */
    public function withRoundRobinLoadBalancingPolicy()
    {
        $this->loadBalancingPolicy = self::LOAD_BALANCING_ROUND_ROBIN;
        return $this;
    }

    /**
     * Configures this cluster to use a datacenter aware round robin load balancing policy.
     *
     * @param string  $localDatacenter                          Name of the local datacenter
     * @param integer $hostPerRemoteDatacenter                  Maximum number of hosts to try in remote datacenters
     * @param boolean $useRemoteDatacenterForLocalConsistencies Allow using hosts from remote datacenters to execute statements with local consistencies
     *
     * @return Cassandra\Cluster\Builder self
     */
    public function withDatacenterAwareRoundRobinLoadBalancingPolicy($localDatacenter, $hostPerRemoteDatacenter, $useRemoteDatacenterForLocalConsistencies)
    {
        $localDatacenter                          = (string) $localDatacenter;
        $hostPerRemoteDatacenter                  = (int) $hostPerRemoteDatacenter;
        $useRemoteDatacenterForLocalConsistencies = (bool) $useRemoteDatacenterForLocalConsistencies;

        if ($hostPerRemoteDatacenter < 0) {
            throw new InvalidArgumentException(sprintf("Number of hosts per remote datacenter cannot be negative, %s given", $hostPerRemoteDatacenter));
        }

        $this->loadBalancingPolicy                      = self::LOAD_BALANCING_DC_AWARE_ROUND_ROBIN;
        $this->localDatacenter                          = $localDatacenter;
        $this->hostPerRemoteDatacenter                  = $hostPerRemoteDatacenter;
        $this->useRemoteDatacenterForLocalConsistencies = $useRemoteDatacenterForLocalConsistencies;
        return $this;
    }

    /**
     * Enable token aware routing
     *
     * @param boolean $enabled Whether to enable token aware routing (default: `true`)
     *
     * @return Cassandra\Cluster\Builder self
     */
    public function withTokenAwareRouting($enabled = true)
    {
        $this->useTokenAwareRouting = (bool) $enabled;
        return $this;
    }

    /**
     * Configures cassandra authentication
     *
     * @param string $username Username
     * @param string $password Password
     *
     * @return Cassandra\Cluster\Builder self
     */
    public function withCredentials($username, $password)
    {
        $this->username = (string) $username;
        $this->password = (string) $password;
        return $this;
    }

    /**
     * Timeout used for establishing TCP connections
     *
     * @param float $timeout Timeout value in seconds, can be fractional
     *
     * @return Cassandra\Cluster\Builder self
     */
    public function withConnectTimeout($timeout)
    {
        $this->connectTimeout = (float) $timeout;
        return $this;
    }

    /**
     * Timeout used for waiting for a response from a node
     *
     * @param float $timeout Timeout value in seconds, can be fractional
     *
     * @return Cassandra\Cluster\Builder self
     */
    public function withRequestTimeout($timeout)
    {
        $this->requestTimeout = (float) $timeout;
        return $this;
    }

    /**
     * Set up ssl context
     * @param SSLContext $context a preconfigured ssl context
     */
    public function withSSLContext(SSLContext $context)
    {
        $this->sslContext = $context;
        return $this;
    }
}
