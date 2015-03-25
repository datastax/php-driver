<?php

namespace Cassandra;

use Cassandra\Exception\InvalidArgumentException;

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

        $rows   = array();
        foreach (cassanrda_rows_from_result(cassandra_future_get_result($this->resource)) as $row) {
            $rows[]= new Row($row);
        }

        $this->rows     = new Rows($rows);
        $this->resource = null;
        return $this->rows;
    }
}
