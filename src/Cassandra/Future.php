<?php

namespace Cassandra;

/**
 * Futures are returns from asynchronous methods.
 *
 * @see Cassandra\Session::executeAsync()
 * @see Cassandra\Session::prepareAsync()
 * @see Cassandra\Session::closeAsync()
 */
interface Future
{
    /**
     * Waits for a given future resource to resolve and throws errors if any
     *
     * @throws Cassandra\Exception\InvalidArgumentException
     * @throws Cassandra\Exception\TimeoutException
     *
     * @return mixed a value that the future has been resolved with
     */
    function get($timeout = null);
}
