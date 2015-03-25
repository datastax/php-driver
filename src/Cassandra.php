<?php

use Cassandra\Cluster\Builder as ClusterBuilder;
use Cassandra\SSLOptions\Builder as SSLOptionsBuilder;

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

    const SSL_VERIFY_NONE          = 0;
    const SSL_VERIFY_PEER_CERT     = 1;
    const SSL_VERIFY_PEER_IDENTITY = 2;

    /**
     * Returns a builder for customizing the cluster
     *
     * @return Cassandra\Cluster\Builder a Cluster Builder instance
     */
    public static function cluster()
    {
        return new ClusterBuilder();
    }

    /**
     * Returns a builder for ssl options
     *
     * @return Cassanrda\SSLOptions\Builder a SSLOptions Builder instance
     */
    public static function ssl()
    {
        return new SSLOptionsBuilder();
    }
}
