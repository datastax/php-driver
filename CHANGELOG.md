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
