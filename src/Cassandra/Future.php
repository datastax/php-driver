<?php

namespace Cassandra;

use Cassandra\Exception\LogicException;

final class Future
{
    private $resource;

    public function __construct($resource)
    {
        $this->resource = $resource;
    }

    /**
     * Waits for a given future resource to resolve and throws errors if any
     *
     * @access private
     *
     * @throws Cassandra\Exception\LibraryException
     *
     * @return boolean wether the future resolved or not (if timeout given)
     */
    public function wait($timeout = null)
    {
        if (is_null($timeout)) {
            cassandra_future_wait($this->resource);
        } elseif (!is_numeric($timeout) || $timeout <= 0) {
            throw new InvalidArgumentException(sprintf(
                "Timeout must be positive number, %s given",
                var_export($timeout, true)
            ));
        } else {
            if (!cassandra_future_wait_timed($this->resource, $timeout)) {
                return false;
            }
        }

        $code = cassandra_future_error_code($this->resource);

        if ($code === 0) {
            return true;
        }

        $message = cassandra_future_error_message($this->resource);

        Util::throwException($code, $message);
    }

    /**
     * Returns a Result instance or null if future resolve with an error
     *
     * @return  Cassandra\Result  a Result instance or null
     */
    public function result()
    {
        $result = cassandra_future_get_result($this->resource);

        if (is_null($result)) {
            $code = cassandra_future_error_code($this->resource);

            if ($code === 0) {
                throw LogicException("Attempting to get result from future that isn't supposed to return one");
            }

            $message = cassandra_future_error_message($this->resource);

            Util::throwException($code, $message);
        }

        return new PagedResult($result);
    }
}
