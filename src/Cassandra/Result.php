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
}
