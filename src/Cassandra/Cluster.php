<?php

namespace Cassandra;

interface Cluster
{
  /**
   * Creates a new Session instance
   *
   * @param  string  $keyspace  Optional keyspace name
   *
   * @return  Cassandra\Session  Session instance
   */
  public function connect($keyspace = null);
}
