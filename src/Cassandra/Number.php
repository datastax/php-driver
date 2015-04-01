<?php

namespace Cassandra;

interface Number
{
    /**
     * Bitwise AND
     * @param  Cassandra\Number|int|float $number number
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float result
     */
    function and($number);

    /**
     * Bitwise OR
     * @param  Cassandra\Number|int|float $number number
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float result
     */
    function or($number);

    /**
     * Bitwise XOR
     * @param  Cassandra\Number|int|float $number number
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float result
     */
    function xor($number);

    /**
     * Set the bit at a given index
     * @param int $index an index
     * @throws Cassandra\Exception\OutOfRangeException
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float this number
     */
    function setBit($index);

    /**
     * Unset the bit at a given index
     * @param int $index an index
     * @throws Cassandra\Exception\OutOfRangeException
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float this number
     */
    function unsetBit($index);

    /**
     * Returns whether the bit at a given index is set
     * @param int $index an index
     * @throws Cassandra\Exception\OutOfRangeException
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return boolean  whether the bit at a given index is set
     */
    function issetBit($index);

    /**
     * Invert all bits, also known as one's complement
     * @return Cassandra\Number|int|float inverted number
     */
    function invertBits();

    /**
     * Compare with the other number:
     * * `0`  - equal
     * * `-1` - less than
     * * `1`  - greater than
     * @param  Cassandra\Number|int|float $number number
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return int                                result
     */
    function compare($number);

    /**
     * Absolute value
     * @return Cassandra\Number|int|float result
     */
    function abs();

    /**
     * Negation
     * @return Cassandra\Number|int|float         negative
     */
    function neg();

    /**
     * Addition
     * @param  Cassandra\Number|int|float $number summand
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float         sum
     */
    function add($number);

    /**
     * Multiplication
     * @param  Cassandra\Number|int|float $number multiplier
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float         product
     */
    function mul($number);

    /**
     * Subtraction
     * @param  Cassandra\Number|int|float $number subtrahend
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float         difference
     */
    function sub($number);

    /**
     * Division
     * @param  Cassandra\Number|int|float $number divisor
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float         quotient
     */
    function div($number);

    /**
     * Modulation
     * @param  Cassandra\Number|int|float $number divisor
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int               remainder
     */
    function mod($number);

    /**
     * Get factorial
     * @param  Cassandra\Number|int       $number factor
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float         result
     */
    function fact($number);

    /**
     * Take square root of this number
     * @return Cassandra\Number|int|float         square root
     */
    function sqrt();

    /**
     * Take the nth root
     * @param  int                        $number degree
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float         root
     */
    function root($number);

    /**
     * Raise number into power
     * @param  Cassandra\Number|int       $number exponent
     * @throws Cassandra\Exception\InvalidArgumentException
     * @return Cassandra\Number|int|float         power
     */
    function pow($number);

    /**
     * Whether this number is positive
     * @return boolean whether this number is positive
     */
    function isPositive();

    /**
     * Convert this number to int
     * @return Cassandra\Number|int       $number integer
     */
    function ceil();

    /**
     * Convert this number to int
     * @return Cassandra\Number|int       $number integer
     */
    function floor();
}
