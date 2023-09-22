# 1.4.0

Features:

* [joeyhub] Nuked blob, in future objects should not represent types as it gobbles up memory and CPU plus makes the code 10 times larger than it needs to be.

# 1.3.2

Features:

* [PHP-155] Support for PHP 7.1

Bug Fixes:

* [PHP-189] Set of map : bad hash calculation
* [PHP-172] php_driver_value_hash hashes IS_TRUE and IS_FALSE to 1 in PHP7
* [PHP-163] Disable hostname resolution if compiling against libuv 0.10
* [PHP-159] Driver version Number (composer)
* [PHP-142] Memory Leak in Cassandra\Timestamp::toDateTime() and Cassandra\UserTypeValue::values()
* [PHP-118] Unable to bind list values using global type constants

Documentation:

* [PHP-196] Update building and testing documentation
* [PHP-191] Updates to README files
* [PHP-117] Broken links in documentor-generated docs
* [PHP-102] Documentation improvements

# 1.3.1

Bug Fixes:

* [PHP-170] Duration nanos should support a signed 64-bit range

Documentation:

* [PHP-184] Remove {@inheritDoc} from method parameters
* [PHP-183] Remove references to SimpleStatement and ExecutionOptions from docs,
  features, and most tests

# 1.3.0

Features:

* Added support for the `duration` type
* Added support to pass a query string  directly to `Session::execute()` and
  `Session::executeAsync()`
* `ExecutionOptions` has been deprecated in favor of using an array

Bug Fixes:

* [PHP-86] Decimal __toString method
* [PHP-109] Cassandra solr_query not working after upgrading to DSE 5
* [PHP-114] pecl install compiler output is dominated by C90 warnings
* [PHP-119] Driver crashes upon getting type of column
* [PHP-143] Memory leak in `FutureSession`
* [PHP-144] Fix persistent sessions reference counting issue
* [PHP-147] Unable to create Bigint with PHP_INT_MIN

# 1.2.2

Bug Fixes:

* [PHP-88] \Cassandra\Timestamp::toDateTime segfault with PHP7
* [PHP-112] Freeing a null future as result of a failure in \Cassandra\DefaultSession::executeAsync()
* [PHP-115] \Cassandra\UUID returning duplicate UUIDs

# 1.2.1

Bug Fixes:

* [PHP-113] pecl install of 1.2.0 fails because sourcecode is missing FutureRows.h

# 1.2.0

Features:

* Added support for `tinyint` and `smallint` types
* Added support for `date` and `time` types
* Added support for secondary index metadata
* Added support for UDF/UDA metadata
* Added support for materialized view metadata
* Added blacklist and whitelist datacenter and host policies

Bug Fixes:

* [PHP-100] Whitelist/Blacklist LB setting should update the cluster builder hash key
* [PHP-101] Memory leak when paging results

# 1.1.0

Features:

* Added support for PHP 7
* Added support for UDTs and tuples
* Added support for nested collections
* Added access to raw paging token (via `Cassandra\Rows::pagingStateToken()`)
* Added support for client-side timestamps and timestamp generators
* Added support for retry policys (includes logging, downgrading and fallthrough)
* Added the ability to disable schema metadata updates (via `Cassandra\Cluster\withSchemaMetdata()`)
* Added support for using named arguments with `Cassandra\SimpleStatement`
* Changed the default consistency to `LOCAL_ONE` (from `ONE`)

Bug Fixes:

* [PHP-70] Fixed invalid encoding of decimals and varints

# 1.0.1

Features:

* Added support for the 2.2.x release of the C/C++ driver

Bug Fixes:

* [PHP-53] Error when inserting a "0" Decimal

# 1.0.0

Features:

* Added schema metadata support (via `Cassandra\Session::schema()`).
* Added types API (via `Cassandra\Type` static methods).

Bug Fixes:

* [PHP-40] Fixed invalid pointer being free'd
* [PHP-43] Fixed timeout handling in `Cassandra\ExecutionOptions`
* [PHP-44] Fixed memory leak

# 1.0.0-rc

Features:

* Added `Cassandra\Collection::remove()`.
* Added `Cassandra\Blob::toBinaryString()`.
* Added ability to configure io threads and connections per host.
* Enabled latency aware routing by default and added a way of disabling it.
* Enabled TCP nodelay, added a way of disabling it.
* Added configuration for TCP keepalive.

Bug Fixes:

* [PHP-30] Segfault when using FutureRows
* [PHP-31] Segfault during deserialization of NULL values
* [PHP-33] Fixed timestamps generation on 32bit systems
* [PHP-37] Removed extra null byte when `Cassandra\Blob` created from string
* [PHP-41] Fix memory leak when using batch statements

# 1.0.0-beta

Features:

* Persistent sessions (can be disabled via `Cassandra\Cluster\Builder::withPersistentSessions()`).
* Windows support.
* Configurable log location (via `cassandra.log` ini setting).
* Configurable log level (via `cassandra.log_level` ini setting).
* Upgraded to C/C++ driver v2.0.
* Added math functions to numeric types and `Cassandra\Numeric` interface.
* All classes moved into the extension.
* Added stub files for IDEs.

Bug Fixes:

* [PHP-18] Decimal parsing failure

# 1.0.0-alpha

Features:

* All pre 2.1 Cassandra datatypes, including sets, lists and maps.
* Simple, prepared and batch statements.
* Results paging.
* Asynchronous IO.
* SSL encryption.
* Credentials authentication.
