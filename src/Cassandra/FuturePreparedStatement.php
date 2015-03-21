<?php

namespace Cassandra;

use Cassandra\Exception\LibraryException;
use Cassandra\Exception\InvalidArgumentException;
use Cassandra\Exception\TimeoutException;

final class FuturePreparedStatement implements Future
{
    private $resource;

    public function __construct($resource)
    {
        $this->resource  = $resource;
        $this->exception = null;
        $this->statement = null;
    }

    public function get($timeout = null)
    {
        if (!is_null($this->exception)) {
            throw $this->exception;
        }

        if (!is_null($this->statement)) {
            return $this->statement;
        }

        if (is_null($timeout)) {
            cassandra_future_wait($this->resource);
        } elseif (!is_numeric($timeout) || $timeout <= 0) {
            throw new InvalidArgumentException(sprintf(
                "Timeout must be positive number, %s given",
                var_export($timeout, true)
            ));
        } else {
            if (!cassandra_future_wait_timed($this->resource, $timeout)) {
                throw new TimeoutException(sprintf(
                    "Unable to resolve future within %d seconds", $timeout
                ));
            }
        }

        $code = cassandra_future_error_code($this->resource);

        if ($code === 0) {
            $this->statement = new PreparedStatement(cassandra_future_get_prepared($this->resource));
            $this->resource  = null;
            return $this->statement;
        }

        $this->exception = new LibraryException(cassandra_future_error_message($this->resource), $code);
        $this->resource  = null;
        throw $this->exception;
    }
}
