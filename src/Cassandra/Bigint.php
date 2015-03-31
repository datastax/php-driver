<?php

namespace Cassandra;

/**
 * A PHP representation of the CQL `bigint` datatype
 */
final class Bigint
{
    /**
     * Creates a new 64bit integer
     * @param string $value integer value as a string
     */
    public function __construct($value) {}

    /**
     * Returns the integer value
     * @return string integer value
     */
    public function value() {}

    /**
     * Returns string representation of the integer value
     * @return string integer value
     */
    public function __toString() {}
}
