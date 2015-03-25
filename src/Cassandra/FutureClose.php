<?php

namespace Cassandra;

use Cassandra\Exception\InvalidArgumentException;

final class FutureClose implements Future
{
    private $resource;
    private $session;
    private $resolved;

    public function __construct($resource, &$session)
    {
        $this->resource =  $resource;
        $this->session  =& $session;
        $this->resolved =  false;
    }

    public function get($timeout = null)
    {
        if ($this->resolved) {
            return;
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

        $this->resource = null;
        $this->session  = null;
        $this->resolved = true;
    }
}
