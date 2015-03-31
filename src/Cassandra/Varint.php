<?php

namespace Cassandra

/**
 * A PHP representation of the CQL `varint` datatype
 */
final class Varint
{
    /**
     * Creates a new variable length integer
     * @param string $value integer value as a string
     */
    public function __construct($value) {}

    /**
     * Returns the integer value
     * @return string integer value
     */
    public function __toString() {}

    /**
     * Returns the integer value
     * @return string integer value
     */
    public function value() {}
}
