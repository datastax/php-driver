<?php

namespace Cassandra;

use Cassandra\Exception\LogicException;
use Cassandra\Exception\LibraryException;

final class FutureRows implements Future
{
    private $resource;
    private $exception;
    private $rows;

    public function __construct($resource)
    {
        $this->resource  = $resource;
        $this->exception = null;
        $this->rows      = null;
    }

    public function get($timeout = null)
    {
        if (!is_null($this->exception)) {
            throw $this->exception;
        }

        if (!is_null($this->rows)) {
            return $this->rows;
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
            $this->rows     = new Rows(cassandra_future_get_result($this->resource));
            $this->resource = null;
            return $this->rows;
        }

        $this->exception = new LibraryException(cassandra_future_error_message($this->resource), $code);
        $this->resource  = null;
        throw $this->exception;
    }
}
