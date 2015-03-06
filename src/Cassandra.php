<?php

use Cassandra\Cluster\Builder;
use Cassandra\Exception\LibraryException;

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
