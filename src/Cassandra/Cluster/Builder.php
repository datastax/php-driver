<?php

namespace Cassandra\Cluster;

final class Builder
{
  /**
   * Cluster resource
   * @var resource
   * @access private
   */
  private $resource;

  /**
   * @access private
   */
  public function __construct()
  {
    $this->resource = cassandra_cluster_new();
  }

  public function __destruct()
  {
  }

  /**
   * Returns a Cluster Instance
   *
   * @return Cassandra\Cluster Cluster instance
   */
  public function build()
  {
    return new Cluster($this->resource);
  }

  /**
   * Configures this cluster to use a round robin load balancing policy.
   *
   * @return Cassandra\Cluster\Builder self
   */
  public function withRoundRobinLoadBalancingPolicy()
  {
    cassandra_cluster_set_load_balance_round_robin($this->resource);
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
    cassandra_cluster_set_load_balance_dc_aware($this->resource, (string) $localDatacenter, (integer) $hostPerRemoteDatacenter, (bool) $useRemoteDatacenterForLocalConsistencies);
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
    cassandra_cluster_set_token_aware_routing($this->resource, (bool) $enabled);
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
    cassandra_cluster_set_credentials($this->resource, (string) $username, (string) $password);
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
    cassandra_cluster_set_connect_timeout($this->resource, (float) $timeout);
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
    cassandra_cluster_set_request_timeout($this->resource, (float) $timeout);
    return $this;
  }

  /**
   * Set up ssl context
   * @param SSLContext $context a preconfigured ssl context
   */
  public function withSSLContext(SSLContext $context)
  {
    if ($context instanceof StandardSSLContext) {
      cassandra_cluster_set_ssl($this->resource, $context->resource());
    }

    return $this;
  }
}
