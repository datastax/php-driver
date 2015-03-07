<?php

namespace Cassandra;

final class Timestamp
{
    public function __construct($time = null, $usec = null);
    public function time();
    public function microtime($get_as_float = false);
    public function toDateTime();
    public function __toString();
}
