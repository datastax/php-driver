<?php

namespace Cassandra;

/**
 * A session is used to prepare and execute statements.
 *
 * @see Cassandra\Cluster::connect()
 */
interface Session
{
    /**
     * Executes a given statement and returns a result
     *
     * @param  Statement        $statement statement to be executed
     * @param  ExecutionOptions $options   execution options
     *
     * @throws Cassandra\Exception
     *
     * @return Result               execution result
     */
    function execute(Statement $statement, ExecutionOptions $options = null);

    /**
     * Executes a given statement and returns a future result
     *
     * Note that this method ignores ExecutionOptions::$timeout option, you can
     * provide one to Future::get() instead.
     *
     * @param  Statement $statement statement to be executed
     * @param  array     $options   execution options
     *
     * @return Cassandra\Future     future result
     */
    function executeAsync(Statement $statement, ExecutionOptions $options = null);

    /**
     * Creates a prepared statement from a given CQL string
     *
     * Note that this method only uses the ExecutionOptions::$timeout option,
     * all other options will be ignored.
     *
     * @param  string $cql        CQL statement string
     * @param  array  $options    execution options
     *
     * @throws Cassandra\Exception
     *
     * @return PreparedStatement  prepared statement
     */
    function prepare($cql, ExecutionOptions $options = null);

    /**
     * Asynchronously prepares a statement and returns a future prepared statement
     *
     * Note that all options passed to this method will be ignored.
     *
     * @param  string $cql       CQL string to be prepared
     * @param  array  $options   preparation options
     * @return Cassandra\Future  statement
     */
    function prepareAsync($cql, ExecutionOptions $options = null);
    //
    // /**
    //  * Closes current session and all of its connections
    //  *
    //  * @return void
    //  */
    // function close();
    //
    // /**
    //  * Asynchronously closes current session once all pending requests have finished
    //  *
    //  * @return Cassandra\Future  future
    //  */
    // function closeAsync();
}
