<?php

use Cassandra\Cluster\Builder as ClusterBuilder;
use Cassandra\SSLOptions\Builder as SSLOptionsBuilder;

/**
 * The main entry point to the PHP Driver for Apache Cassandra
 *
 * Use Cassandra::cluster() to build a cluster instance.
 * Use Cassandra::ssl() to build SSL options instance.
 */
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

    const BATCH_LOGGED   = 0;
    const BATCH_UNLOGGED = 1;
    const BATCH_COUNTER  = 2;

    const VERSION = '1.0.0.alpha';

    /**
     * Returns a Cluster Builder
     *
     * @return Cassandra\Cluster\Builder a Cluster Builder instance
     */
    public static function cluster()
    {
        return new ClusterBuilder();
    }

    /**
     * Returns SSL Options Builder
     *
     * @return Cassanrda\SSLOptions\Builder a SSLOptions Builder instance
     */
    public static function ssl()
    {
        return new SSLOptionsBuilder();
    }
}
