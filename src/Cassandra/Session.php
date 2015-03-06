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
     * @throws Cassandra\Exception
     *
     * @return Result               execution result
     */
    function execute(Statement $statement, array $options = array());

    /**
     * Executes a given statement and returns a future result
     *
     * @param  Statement $statement statement to be executed
     * @param  array     $options   execution options
     *
     * @return Cassandra\Future     future result
     */
    function executeAsync(Statement $statement, array $options = array());

    /**
     * Creates a prepared statement from a given CQL string
     *
     * @param  string $cql        CQL statement string
     * @param  array  $options    execution options
     *
     * @throws Cassandra\Exception
     *
     * @return PreparedStatement  prepared statement
     */
    function prepare($cql, array $options = array());

    /**
     * Asynchronously prepares a statement and returns a future prepared statement
     *
     * @param  string $cql       CQL string to be prepared
     * @param  array  $options   preparation options
     * @return Cassandra\Future  statement
     */
    function prepareAsync($cql, array $options = array());
}
