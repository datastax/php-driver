<?php

namespace Cassandra;

interface Result extends \Countable, \ArrayAccess, \IteratorAggregate
{
    /**
     * @return  boolean  whether this is the last page or not
     */
    function isLastPage();

    /**
     * @return  Cassandra\Result|null  loads and returns next result page
     */
    function nextPage();

    /**
     * @return  Cassandra\Row|null  returns first row if any
     */
    function first();
}
