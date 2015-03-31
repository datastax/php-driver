<?php

namespace Cassandra;

/**
 * A PHP representation of the CQL `uuid` datatype
 */
final class Uuid
{
    /**
     * Creates a uuid from a given uuid string or a random one.
     * @param string $uuid A uuid string
     */
    public function __construct($uuid = null) {}

    /**
     * Returns this uuid as string
     * @return string uuid
     */
    public function __toString() {}

    /**
     * Returns this uuid as string
     * @return string uuid
     */
    public function uuid() {}

    /**
     * Returns the version of this uuid
     * @return integer version of this uuid
     */
    public function version() {}
}
