<?php

namespace Cassandra;

final class FutureException implements Future
{
    private $exception;

    public function __construct($exception)
    {
        $this->exception = $exception;
    }

    public function get($timeout = null)
    {
        throw $this->exception;
    }
}
