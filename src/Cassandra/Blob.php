<?php

namespace Cassandra;

final class Blob
{
    private $bytes;

    public function __construct($bytes);
    public function bytes();
    public function __toString();
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
