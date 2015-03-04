<?php

namespace Cassandra;

use Cassandra\Exception\LibraryException;

final class Future
{
    /**
     * Waits for a given future resource to resolve and throws errors if any
     *
     * @access private
     *
     * @param  resource $resource A future resource
     *
     * @throws Cassandra\Exception\LibraryException
     *
     * @return void
     */
    public static function wait($resource)
    {
        $code = cassandra_future_error_code($resource);

        if ($code === 0) {
            return;
        }

        $message = cassandra_future_error_message($resource);
        cassandra_future_free($resource);

        Cassandra::throwException($code, $message);
    }
}
