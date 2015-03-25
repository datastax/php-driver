<?php

namespace Cassandra;

use Cassandra\Exception\LibraryException;
use Cassandra\Exception\InvalidArgumentException;
use Cassandra\Exception\TimeoutException;

final class FuturePreparedStatement implements Future
{
    private $resource;
    private $statement;

    public function __construct($resource)
    {
        $this->resource  = $resource;
        $this->statement = null;
    }

    public function get($timeout = null)
    {

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
            cassandra_future_wait_timed($this->resource, $timeout);
        }

        $this->statement = new PreparedStatement(cassandra_future_get_prepared($this->resource));
        $this->resource  = null;
        return $this->statement;
    }
}
