PHP_ARG_WITH(cassandra, Enable Cassandra extension,
[  --with-cassandra[=DIR] Enable the Cassandra extension.])

if test -z "$PHP_GMP"; then
  PHP_ARG_WITH(gmp, GNU MP install dir,
  [  --with-gmp=DIR Specify GNU MP install dir.])
fi

if test "$PHP_CASSANDRA" != "no"; then
  PHP_SUBST(CASSANDRA_SHARED_LIBADD)
  PHP_NEW_EXTENSION(cassandra, php_cassandra.c exceptions/exception.c \
    exceptions/invalid_argument.c exceptions/runtime.c exceptions/timeout.c \
    exceptions/logic.c exceptions/server.c util/bytes.c util/collections.c \
    util/inet.c util/math.c util/uuid_gen.c types/float.c types/bigint.c \
    types/blob.c types/decimal.c types/inet.c types/uuid_interface.c \
    types/uuid.c types/timestamp.c types/timeuuid.c  types/varint.c \
    types/set.c types/map.c types/collection.c, $ext_shared)

  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(cassandra, spl)
  ])

  PHP_ADD_BUILD_DIR([$ext_builddir/exceptions], 1)
  PHP_ADD_INCLUDE([$ext_builddir/exceptions])
  PHP_ADD_INCLUDE([$ext_srcdir/exceptions])

  PHP_ADD_BUILD_DIR([$ext_builddir/types], 1)
  PHP_ADD_INCLUDE([$ext_builddir/types])
  PHP_ADD_INCLUDE([$ext_srcdir/types])

  if test "$PHP_GMP" != "no"; then
    if test -f $PHP_GMP/include/gmp.h; then
      GMP_DIR=$PHP_GMP
    fi
  else
    for i in /usr/local /usr; do
      if test -f $i/include/gmp.h; then
        GMP_DIR=$i
      fi
    done
  fi

  if test -z "$GMP_DIR"; then
    ac_extra=
  else
    ac_extra=-L$GMP_DIR/$PHP_LIBDIR
  fi

  PHP_CHECK_LIBRARY(gmp, __gmp_version,
    [
      AC_DEFINE(HAVE_GMPLIB,1,[ ])
    ],
    [
      AC_MSG_ERROR([Unable to load libgmp])
    ],
    [
      $ac_extra
    ]
  )

  if test -n "$GMP_DIR"; then
    PHP_ADD_LIBPATH($GMP_DIR/$PHP_LIBDIR, CASSANDRA_SHARED_LIBADD)
    PHP_ADD_INCLUDE($GMP_DIR/include)
  fi

  PHP_ADD_LIBRARY(gmp,, CASSANDRA_SHARED_LIBADD)

  if test "$PHP_CASSANDRA" != "yes"; then
    if test -f $PHP_CASSANDRA/include/cassandra.h; then
      CPP_DRIVER_DIR=$PHP_CASSANDRA
    fi
  else
    for i in /usr/local /usr; do
      if test -f $i/include/cassandra.h; then
        CPP_DRIVER_DIR=$i
      fi
    done
  fi

  if test -z "$CPP_DRIVER_DIR"; then
    ac_extra=
  else
    ac_extra=-L$CPP_DRIVER_DIR/$PHP_LIBDIR
  fi

  PHP_CHECK_LIBRARY(cassandra, cass_cluster_new,
    [
      AC_DEFINE(HAVE_CASSANDRALIB,1,[ ])
    ],
    [
      AC_MSG_ERROR([Unable to load libcassandra])
    ],
    [
      $ac_extra
    ]
  )

  if test -n "$CPP_DRIVER_DIR"; then
    PHP_ADD_LIBPATH($CPP_DRIVER_DIR/$PHP_LIBDIR, CASSANDRA_SHARED_LIBADD)
    PHP_ADD_INCLUDE($CPP_DRIVER_DIR/include)
  fi

  PHP_ADD_LIBRARY(cassandra,, CASSANDRA_SHARED_LIBADD)

  CASSANDRA_SHARED_LIBADD="$CASSANDRA_SHARED_LIBADD $LIBS"
fi
