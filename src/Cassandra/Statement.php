<?php

namespace Cassandra;

interface Statement
{
    /**
     * @access private
     *
     * @return resource  Actual statement resource
     */
    function resource($consistency, $serialConsistency, $pageSize, array $arguments = null);
}
