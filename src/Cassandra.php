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

    public static function assertNoError($code)
    {
        if ($code === 0) {
            return;
        }

        self::throwException($code, cassandra_error_message($code));
    }

    public static function throwException($code, $message)
    {
        throw new LibraryException($message, $code);
    }
}
