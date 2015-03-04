<?php

final class StandardCluster implements Cluster
{
  /**
   * Cluster resource
   * @var resource
   */
  private $resource;

  /**
   * @access private
   * @param resource $resource Cluster resource
   */
  public function __construct($resource)
  {
    $this->resource = $resource;
  }

  public function __destruct()
  {
    cassandra_cluster_free($this->resource);
    $this->resource = null;
  }

  /**
   * {@inheritDoc}
   */
  public function connect($keyspace)
  {
    
  }
}
