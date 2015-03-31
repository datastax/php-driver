<?php

namespace Cassandra

/**
 * A PHP representation of the CQL `decimal` datatype
 *
 * The actual value of a decimal is `$value * pow(10, $scale * -1)`
 */
final class Decimal
{
    /**
     * Creates a decimal from a given decimal string:
     * <code>
     * $decimal = new Cassandra\Decimal("1313123123.234234234234234234123");
     *
     * $this->assertEquals(21, $decimal->scale());
     * $this->assertEquals("1313123123234234234234234234123", $decimal->value());
     * </code>
     * @param  string  $value  Any decimal string
     */
    public function __construct($value) {}

    /**
     * String representation of this decimal
     * @return string Decimal value
     */
    public function __toString() {}

    /**
     * Numeric value of this decimal as string
     * @return string Numeric value
     */
    public function value() {}

    /**
     * Scale of this decimal as int
     * @return int Scale
     */
    public function scale() {}
}
