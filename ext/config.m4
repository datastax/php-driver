PHP_ARG_WITH(cassandra, Enable Cassandra extension,
[  --with-cassandra[=DIR]    Enable the Cassandra extension.])

AC_MSG_CHECKING([for supported PHP version])
if test -z "$PHP_VERSION"; then
  if test -z "$PHP_CONFIG"; then
    AC_MSG_ERROR([php-config not found])
  fi
  PHP_CASSANDRA_FOUND_PHP_VERSION=`$PHP_CONFIG --version`
else
  PHP_CASSANDRA_FOUND_PHP_VERSION="$PHP_VERSION"
fi

PHP_CASSANDRA_FOUND_PHP_VERSION_NUMBER=`echo "${PHP_CASSANDRA_FOUND_PHP_VERSION}" | $AWK 'BEGIN { FS = "."; } { printf "%d", ([$]1 * 100 + [$]2) * 100 + [$]3;}'`
if test -z "$PHP_CASSANDRA_FOUND_PHP_VERSION_NUMBER"; then
  AC_MSG_ERROR([failed to detect PHP version, please report])
fi

if test "$PHP_CASSANDRA_FOUND_PHP_VERSION_NUMBER" -lt "50600"; then
  AC_MSG_ERROR([not supported. PHP version 5.6.0+ required (found $PHP_CASSANDRA_FOUND_PHP_VERSION)])
else
  AC_MSG_RESULT([supported ($PHP_CASSANDRA_FOUND_PHP_VERSION)])
fi

if test -z "$PHP_UV"; then
  PHP_ARG_WITH(uv, libuv install dir,
  [  --with-uv=DIR           Specify libuv install dir.])
fi

if test -z "$PHP_GMP"; then
  PHP_ARG_WITH(gmp, GNU MP install dir,
  [  --with-gmp=DIR          Specify GNU MP install dir.])
fi

if test "$PHP_CASSANDRA" != "no"; then
  CASSANDRA_CLASSES="\
    src/Core.c \
    src/Aggregate.c \
    src/BatchStatement.c \
    src/Bigint.c \
    src/Blob.c \
    src/Cluster.c \
    src/Cluster/Builder.c \
    src/Collection.c \
    src/Column.c \
    src/Custom.c \
    src/Date.c \
    src/Decimal.c \
    src/DefaultAggregate.c \
    src/DefaultCluster.c \
    src/DefaultColumn.c \
    src/DefaultFunction.c \
    src/DefaultIndex.c \
    src/DefaultKeyspace.c \
    src/DefaultMaterializedView.c \
    src/DefaultSchema.c \
    src/DefaultSession.c \
    src/DefaultTable.c \
    src/Duration.c \
    src/Exception.c \
    src/ExecutionOptions.c \
    src/Float.c \
    src/Function.c \
    src/Future.c \
    src/FutureClose.c \
    src/FuturePreparedStatement.c \
    src/FutureRows.c \
    src/FutureSession.c \
    src/FutureValue.c \
    src/Index.c \
    src/Inet.c \
    src/Keyspace.c \
    src/Map.c \
    src/MaterializedView.c \
    src/Numeric.c \
    src/PreparedStatement.c \
    src/RetryPolicy.c \
    src/Rows.c \
    src/Schema.c \
    src/Session.c \
    src/Set.c \
    src/SimpleStatement.c \
    src/Smallint.c \
    src/SSLOptions.c \
    src/SSLOptions/Builder.c \
    src/Statement.c \
    src/Table.c \
    src/Time.c \
    src/Timestamp.c \
    src/TimestampGenerator.c \
    src/TimestampGenerator/Monotonic.c \
    src/TimestampGenerator/ServerSide.c \
    src/Timeuuid.c \
    src/Tinyint.c \
    src/Tuple.c \
    src/Type.c \
    src/UserTypeValue.c \
    src/Uuid.c \
    src/UuidInterface.c \
    src/Value.c \
    src/Varint.c \
  ";

  CASSANDRA_EXCEPTIONS="\
    src/Exception/AlreadyExistsException.c \
    src/Exception/AuthenticationException.c \
    src/Exception/ConfigurationException.c \
    src/Exception/DivideByZeroException.c \
    src/Exception/DomainException.c \
    src/Exception/ExecutionException.c \
    src/Exception/InvalidArgumentException.c \
    src/Exception/InvalidQueryException.c \
    src/Exception/InvalidSyntaxException.c \
    src/Exception/IsBootstrappingException.c \
    src/Exception/LogicException.c \
    src/Exception/OverloadedException.c \
    src/Exception/ProtocolException.c \
    src/Exception/RangeException.c \
    src/Exception/ReadTimeoutException.c \
    src/Exception/RuntimeException.c \
    src/Exception/ServerException.c \
    src/Exception/TimeoutException.c \
    src/Exception/TruncateException.c \
    src/Exception/UnauthorizedException.c \
    src/Exception/UnavailableException.c \
    src/Exception/UnpreparedException.c \
    src/Exception/ValidationException.c \
    src/Exception/WriteTimeoutException.c
  ";

  CASSANDRA_RETRY_POLICIES="\
    src/RetryPolicy/DefaultPolicy.c \
    src/RetryPolicy/DowngradingConsistency.c \
    src/RetryPolicy/Fallthrough.c \
    src/RetryPolicy/Logging.c
  ";

  CASSANDRA_TYPES="\
    src/Type/Collection.c \
    src/Type/Custom.c \
    src/Type/Map.c \
    src/Type/Scalar.c \
    src/Type/Set.c \
    src/Type/Tuple.c \
    src/Type/UserType.c
  ";

  CASSANDRA_UTIL="\
    util/bytes.c \
    util/collections.c \
    util/consistency.c \
    util/future.c \
    util/hash.c \
    util/inet.c \
    util/math.c \
    util/ref.c \
    util/result.c \
    util/types.c \
    util/uuid_gen.c \
  ";

  case $(uname -s) in
    Linux)
      CASSANDRA_CFLAGS="-Wall -Wextra -Wno-long-long -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-result -Wno-variadic-macros -Wno-extra-semi -pthread"
      ;;
    Darwin)
      CASSANDRA_CFLAGS="-Wall -Wextra -Wno-long-long -Wno-deprecated-declarations -Wno-unused-parameter -Wno-unused-result -Wno-variadic-macros -Wno-extra-semi"
      ;;
  esac

  PHP_NEW_EXTENSION(cassandra,
    php_driver.c \
    $CASSANDRA_CLASSES \
    $CASSANDRA_EXCEPTIONS \
    $CASSANDRA_RETRY_POLICIES \
    $CASSANDRA_TYPES \
    $CASSANDRA_UTIL,
    $ext_shared, , $CASSANDRA_CFLAGS)
  PHP_ADD_BUILD_DIR($ext_builddir/src)
  PHP_ADD_BUILD_DIR($ext_builddir/src/Cluster)
  PHP_ADD_BUILD_DIR($ext_builddir/src/Exception)
  PHP_ADD_BUILD_DIR($ext_builddir/src/SSLOptions)
  PHP_ADD_BUILD_DIR($ext_builddir/src/Type)
  PHP_ADD_BUILD_DIR($ext_builddir/src/RetryPolicy)
  PHP_ADD_BUILD_DIR($ext_builddir/src/TimestampGenerator)
  PHP_ADD_BUILD_DIR($ext_builddir/util)
  PHP_SUBST(CASSANDRA_SHARED_LIBADD)
  PHP_SUBST(CASSANDRA_CFLAGS)

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

  if test "$PHP_UV" != "no"; then
    if test -f $PHP_UV/include/uv.h; then
      UV_DIR=$PHP_UV
    fi
  else
    for i in /usr/local /usr; do
      if test -f $i/include/uv.h; then
        UV_DIR=$i
      fi
    done
  fi

  if test -z "$UV_DIR"; then
    ac_extra=
  else
    ac_extra=-L$UV_DIR/$PHP_LIBDIR
  fi

  PHP_CHECK_LIBRARY(uv, uv_version,
    [
      AC_DEFINE(HAVE_UVLIB,1,[ ])
    ],
    [
      AC_MSG_ERROR([Unable to load libuv])
    ],
    [
      $ac_extra
    ]
  )

  if test -n "$UV_DIR"; then
    PHP_ADD_LIBPATH($UV_DIR/$PHP_LIBDIR, CASSANDRA_SHARED_LIBADD)
    PHP_ADD_INCLUDE($UV_DIR/include)
  fi

  PHP_ADD_LIBRARY(uv,, CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(m,, CASSANDRA_SHARED_LIBADD)

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

  AC_MSG_CHECKING([for supported DataStax C/C++ driver version])
  if test "$PHP_CASSANDRA" = "yes" -a -x $PKG_CONFIG; then
    CPP_DRIVER_VERSION=`$PKG_CONFIG cassandra --modversion`
    if $PKG_CONFIG cassandra --atleast-version 2.7.0; then
      CPP_DRIVER_INCDIR=`$PKG_CONFIG cassandra --variable=includedir`
      CPP_DRIVER_LIBDIR=`$PKG_CONFIG cassandra --variable=libdir`
      AC_MSG_RESULT([supported ($CPP_DRIVER_VERSION from pkg-config)])
    else
      AC_MSG_ERROR([not supported. Driver version 2.7.0+ required (found $CPP_DRIVER_VERSION)])
    fi

  else
    if test "$PHP_CASSANDRA" != "yes"; then
      if test -f $PHP_CASSANDRA/include/cassandra.h; then
        CPP_DRIVER_INCDIR=$PHP_CASSANDRA/include
        CPP_DRIVER_LIBDIR=$PHP_CASSANDRA/$PHP_LIBDIR
      fi
      if test -f $PHP_CASSANDRA/include/cassandra/cassandra.h; then
        CPP_DRIVER_INCDIR=$PHP_CASSANDRA/include/cassandra
        CPP_DRIVER_LIBDIR=$PHP_CASSANDRA/$PHP_LIBDIR
      fi
    else
      for i in /usr/local /usr; do
        if test -f $i/include/cassandra.h; then
          CPP_DRIVER_INCDIR=$i/include
          CPP_DRIVER_LIBDIR=$i/$PHP_LIBDIR
        fi
      done
    fi

    PHP_CASSANDRA_FOUND_CASSANDRA_VERSION=`$AWK '/CASS_VERSION_MAJOR/ {printf $3"."} /CASS_VERSION_MINOR/ {printf $3"."} /CASS_VERSION_PATCH/ {printf $3}' $CPP_DRIVER_INCDIR/cassandra.h`
    PHP_CASSANDRA_FOUND_CASSANDRA_VERSION_NUMBER=`echo "${PHP_CASSANDRA_FOUND_CASSANDRA_VERSION}" | $AWK 'BEGIN { FS = "."; } { printf "%d", ([$]1 * 100 + [$]2) * 100 + [$]3;}'`
    if test "$PHP_CASSANDRA_FOUND_CASSANDRA_VERSION_NUMBER" -lt "20700"; then
    AC_MSG_ERROR([not supported. Driver version 2.7.0+ required (found $PHP_CASSANDRA_FOUND_CASSANDRA_VERSION)])
    else
      AC_MSG_RESULT([supported ($PHP_CASSANDRA_FOUND_CASSANDRA_VERSION)])
    fi
  fi

  if test -z "$CPP_DRIVER_LIBDIR"; then
    ac_extra=
  else
    ac_extra=-L$CPP_DRIVER_LIBDIR/$PHP_LIBDIR
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

  if test -n "$CPP_DRIVER_INCDIR"; then
    PHP_ADD_LIBPATH($CPP_DRIVER_LIBDIR, CASSANDRA_SHARED_LIBADD)
    PHP_ADD_INCLUDE($CPP_DRIVER_INCDIR)
  fi

  PHP_ADD_LIBRARY(cassandra,, CASSANDRA_SHARED_LIBADD)

  CASSANDRA_SHARED_LIBADD="$CASSANDRA_SHARED_LIBADD $LIBS"
fi
