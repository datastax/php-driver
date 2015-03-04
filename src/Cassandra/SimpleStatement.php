<?php

namespace Cassandra;

final class SimpleStatement implements Statement
{
    private $cql;

    public function __construct($cql)
    {
        $this->cql = $cql;
    }

    public function getCQL()
    {
        return $this->cql;
    }
}
