<?php

use Cassandra\Cluster\Builder;
use Cassandra\Exception\LibraryException;

final class Cassandra
{
    const CONSISTENCY_ANY          = 0;
    const CONSISTENCY_ONE          = 1;
    const CONSISTENCY_TWO          = 2;
    const CONSISTENCY_THREE        = 3;
    const CONSISTENCY_QUORUM       = 4;
    const CONSISTENCY_ALL          = 5;
    const CONSISTENCY_LOCAL_QUORUM = 6;
    const CONSISTENCY_EACH_QUORUM  = 7;
    const CONSISTENCY_SERIAL       = 8;
    const CONSISTENCY_LOCAL_SERIAL = 9;
    const CONSISTENCY_LOCAL_ONE    = 10;

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
