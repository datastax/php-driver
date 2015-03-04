<?php

namespace Cassandra;

interface Session
{
    /**
     * Executes a given statement and returns a result
     *
     * @param  Statement $statement statement to be executed
     * @param  array     $options   execution options
     *
     * @return Result               execution result
     */
    function execute(Statement $statement, array $options = array());

    /**
     * Creates a prepared statement from a given CQL string
     *
     * @param  string $cql        CQL statement string
     * @param  array  $options    execution options
     * @return PreparedStatement  prepared statement
     */
    function prepare($cql, array $options = array());
}
