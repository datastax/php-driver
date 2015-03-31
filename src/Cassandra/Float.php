<?php

namespace Cassandra;

/**
 * A PHP representation of the CQL `float` datatype
 */
final class Float
{
    /**
     * Creates a new float
     * @param string $value float value as a string
     */
    public function __construct($value) {}

    /**
     * Returns the float value
     * @return string float value
     */
    public function value() {}

    /**
     * Returns string representation of the float value
     * @return string float value
     */
    public function __toString() {}
}
