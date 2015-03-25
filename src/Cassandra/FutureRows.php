<?php

namespace Cassandra;

use Cassandra\Exception\LogicException;
use Cassandra\Exception\LibraryException;

final class FutureRows implements Future
{
    private $resource;
    private $rows;

    public function __construct($resource)
    {
        $this->resource  = $resource;
        $this->rows      = null;
    }

    public function get($timeout = null)
    {
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
            cassandra_future_wait_timed($this->resource, $timeout);
        }

        $this->rows     = new Rows(cassandra_future_get_result($this->resource));
        $this->resource = null;
        return $this->rows;
    }
}
