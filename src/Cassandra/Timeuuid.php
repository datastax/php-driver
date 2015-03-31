<?php

namespace Cassandra;

/**
 * A PHP representation of the CQL `timeuuid` datatype
 */
final class Timeuuid
{
    /**
     * Creates a timeuuid from a given timestamp or current time.
     * @param int $timestamp Unix timestamp
     */
    public function __construct($timestamp = null) {}

    /**
     * Returns this timeuuid as string
     * @return string timeuuid
     */
    public function __toString() {}

    /**
     * Returns this timeuuid as string
     * @return string timeuuid
     */
    public function uuid() {}

    /**
     * Returns the version of this timeuuid
     * @return integer version of this timeuuid
     */
    public function version() {}

    /**
     * Unix timestamp
     * @see time
     * @return int seconds
     */
    public function time() {}

    /**
     * Converts current timeuuid to PHP DateTime
     * @return DateTime
     */
    public function toDateTime() {}
}
