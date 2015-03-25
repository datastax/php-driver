<?php

namespace Cassandra;

interface Statement
{
    /**
     * @access private
     *
     * @return resource  Actual statement resource
     */
    function resource(array $arguments = null, $consistency = null, $serialConsistency = null, $pageSize = null);
}
