<?php

use Cassandra\Cluster\Builder;

final class Cassandra
{
  /**
   * Returns a builder for customizing the cluster
   *
   * @return Cassandra\Cluster\Builder a Cluster Builder instance
   */
  public static function cluster()
  {
    return new Builder();
  }
}
