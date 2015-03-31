<?php

namespace Cassandra;

/**
 * A PHP representation of the CQL `blob` datatype
 */
final class Blob
{
    /**
     * Creates a new bytes array
     * @param string $bytes any bytes
     */
    public function __construct($bytes) {}

    /**
     * Returns bytes as a hex string
     * @return string bytes as hexadecimal string
     */
    public function bytes() {}

    /**
     * Returns bytes as a hex string
     * @return string bytes as hexadecimal string
     */
    public function __toString() {}
    /*
    public function toAscii();
    public function toText();
    public function toVarchar();
    public function toBigint();
    public function toCounter();
    public function toInt();
    public function toVarint();
    public function toBoolean();
    public function toDecimal();
    public function toDouble();
    public function toFloat();
    public function toInet();
    public function toTimestamp();
    public function toUuid();
    public function toTimeuuid();
    */
}
