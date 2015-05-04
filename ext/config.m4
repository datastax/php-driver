PHP_ARG_WITH(cassandra, Enable Cassandra extension,
[  --with-cassandra[=DIR] Enable the Cassandra extension.])

if test -z "$PHP_GMP"; then
  PHP_ARG_WITH(gmp, GNU MP install dir,
  [  --with-gmp=DIR Specify GNU MP install dir.])
fi

if test "$PHP_CASSANDRA" != "no"; then
  CASSANDRA_CLASSES="\
    src/Cassandra.c \
    src/Cassandra/Cluster.c \
    src/Cassandra/Cluster/Builder.c \
    src/Cassandra/DefaultCluster.c \
    src/Cassandra/Exception.c \
    src/Cassandra/Exception/DomainException.c \
    src/Cassandra/Exception/InvalidArgumentException.c \
    src/Cassandra/Exception/LogicException.c \
    src/Cassandra/Exception/RuntimeException.c \
    src/Cassandra/Exception/TimeoutException.c \
    src/Cassandra/Exception/ExecutionException.c \
    src/Cassandra/Exception/ReadTimeout.c \
    src/Cassandra/Exception/WriteTimeoutException.c \
    src/Cassandra/Exception/UnavailableException.c \
    src/Cassandra/Exception/TruncateException.c \
    src/Cassandra/Exception/ValidationException.c \
    src/Cassandra/Exception/InvalidQueryException.c \
    src/Cassandra/Exception/InvalidSyntaxException.c \
    src/Cassandra/Exception/UnauthorizedException.c \
    src/Cassandra/Exception/UnpreparedException.c \
    src/Cassandra/Exception/ConfigurationException.c \
    src/Cassandra/Exception/AlreadyExistsException.c \
    src/Cassandra/Exception/AuthenticationException.c \
    src/Cassandra/Exception/ProtocolException.c \
    src/Cassandra/Exception/ServerException.c \
    src/Cassandra/Exception/IsBootstrappingException.c \
    src/Cassandra/Exception/OverloadedException.c \
    src/Cassandra/Exception/DivideByZeroException.c \
    src/Cassandra/Exception/RangeException.c \
    src/Cassandra/Future.c \
    src/Cassandra/FuturePreparedStatement.c \
    src/Cassandra/FutureRows.c \
    src/Cassandra/FutureSession.c \
    src/Cassandra/FutureValue.c \
    src/Cassandra/FutureClose.c \
    src/Cassandra/Session.c \
    src/Cassandra/DefaultSession.c \
    src/Cassandra/SSLOptions.c \
    src/Cassandra/SSLOptions/Builder.c \
    src/Cassandra/Statement.c \
    src/Cassandra/ExecutionOptions.c \
    src/Cassandra/SimpleStatement.c \
    src/Cassandra/PreparedStatement.c \
    src/Cassandra/BatchStatement.c \
    src/Cassandra/Rows.c \
  ";

  CASSANDRA_TYPES="\
    src/Cassandra/Types/Float.c \
    src/Cassandra/Types/Bigint.c \
    src/Cassandra/Types/Blob.c \
    src/Cassandra/Types/Decimal.c \
    src/Cassandra/Types/Inet.c \
    src/Cassandra/Types/Numeric.c \
    src/Cassandra/Types/UuidInterface.c \
    src/Cassandra/Types/Uuid.c \
    src/Cassandra/Types/Timestamp.c \
    src/Cassandra/Types/Timeuuid.c \
    src/Cassandra/Types/Varint.c \
    src/Cassandra/Types/Set.c \
    src/Cassandra/Types/Map.c \
    src/Cassandra/Types/Collection.c \
  ";

  CASSANDRA_UTIL="\
    util/bytes.c \
    util/collections.c \
    util/consistency.c \
    util/future.c \
    util/inet.c \
    util/math.c \
    util/ref.c \
    util/result.c \
    util/uuid_gen.c \
  ";


  PHP_SUBST(CASSANDRA_SHARED_LIBADD)
  PHP_NEW_EXTENSION(cassandra, php_cassandra.c $CASSANDRA_CLASSES \
    $CASSANDRA_TYPES $CASSANDRA_UTIL, $ext_shared)

  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(cassandra, spl)
  ])

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
