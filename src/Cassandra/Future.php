<?php

namespace Cassandra;

interface Future
{
    /**
     * Waits for a given future resource to resolve and throws errors if any
     *
     * @throws Cassandra\Exception\LibraryException
     * @throws Cassandra\Exception\TimeoutException
     *
     * @return mixed a value that the future has been resolved with
     */
    function get($timeout = null);
}
