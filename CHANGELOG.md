# 1.0.0-rc

# Bug Fixes:

* [PHP-30] Segfault when using FutureRows

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
