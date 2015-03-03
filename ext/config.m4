PHP_ARG_ENABLE(cassandra, Enable Cassandra extension,
[--enable-cassandra Enable the Cassandra extension])

if test "$PHP_CASSANDRA" != "no"; then
  PHP_SUBST(CASSANDRA_SHARED_LIBADD)
  PHP_NEW_EXTENSION(cassandra, php_cassandra.c, $ext_shared)
fi
