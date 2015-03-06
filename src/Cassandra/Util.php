<?php

namespace Cassandra;

use Cassandra\Exception\LibraryException;

final class Util
{
    public static function assertNoError($code)
    {
        if ($code === 0) {
            return;
        }

        self::throwException($code, cassandra_error_message($code));
    }

    public static function throwException($code, $message)
    {
        throw new LibraryException($message, $code);
    }
}
