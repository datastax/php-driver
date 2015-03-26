<?php

namespace Cassandra;

use Cassandra\Exception\InvalidArgumentException;

/**
 * This future results is resolved with Cassandra\Rows
 * @see Cassandra\Rows
 */
final class FutureRows implements Future
{
    /**
     * Result future resource
     * @var resource
     */
    private $resource;
    private $session;
    private $statement;
    private $rows;

    public function __construct($resource, $session, $statement)
    {
        $this->resource  = $resource;
        $this->session   = $session;
        $this->statement = $statement;
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

        $this->rows = new Rows(
                          cassandra_future_get_result($this->resource),
                          $this->session, $this->statement
                      );
        $this->resource  = null;
        $this->session   = null;
        $this->statement = null;

        return $this->rows;
    }
}
