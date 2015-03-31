<?php

namespace Cassandra;

/**
 * A PHP representation of the CQL `inet` datatype
 */
final class Inet
{
    /**
     * Creates a new IPv4 or IPv6 inet address
     * @param string $address any IPv4 or IPv6 address
     */
    public function __construct($address) {}

    /**
     * Returns the normalized string representation of the address
     * @return string address
     */
    public function value() {}

    /**
     * Returns the normalized string representation of the address
     * @return string address
     */
    public function __toString() {}
}
