<?php

namespace Cassandra;

abstract class Value
{
    /**
     * Binds this value as an argument to the given statement resource
     *
     * @param  resource  $statementResource  Statement resource to bind to
     * @param  integer   $index              Position of the argument
     */
    abstract public function bind($statementResource, $index);
}
