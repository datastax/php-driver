<?php

namespace Cassandra;

use Cassandra\Exception\InvalidArgumentException;
use Cassandra\Exception\TimeoutException;
use Cassandra\Exception\LogicException;
use Cassandra\Exception\LibraryException;

final class FutureSession implements Future
{
    private $session;
    private $resource;
    private $resolved;
    private $exception;

    public function __construct($resource, Session $session)
    {
        $this->resource  = $resource;
        $this->session   = $session;
        $this->resolved  = false;
    }

    public function get($timeout = null)
    {
        if ($this->resolved) {
            return $this->session;
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

        $this->resolved = true;
        $this->resource = null;
        return $this->session;
    }
}
