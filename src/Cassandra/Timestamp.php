<?php

namespace Cassandra;

/**
 * A PHP representation of the CQL `timestamp` datatype
 */
final class Timestamp
{
    /**
     * Creates a new timestamp from either unix timestamp and microseconds or
     * from the current time by default.
     * @param int $time Unix timestamp
     * @param int $usec Microseconds
     */
    public function __construct($time = null, $usec = null) {}

    /**
     * Unix timestamp
     * @see time
     * @return int seconds
     */
    public function time() {}

    /**
     * Microtime from this timestamp
     * @see microtime
     * @param  boolean      $get_as_float Whther to get this value as float
     * @return float|string               Float or string representation
     */
    public function microtime($get_as_float = false) {}

    /**
     * Converts current timestamp to PHP DateTime
     * @return DateTime
     */
    public function toDateTime() {}

    /**
     * Returns a string representation of this timestamp
     * @return string timestamp
     */
    public function __toString() {}
}
