PHP_ARG_WITH(cassandra, Enable Cassandra extension,
[--with-cassandra[=DIR] Enable the Cassandra extension.])

if test "$PHP_CASSANDRA" != "no"; then
  PHP_SUBST(CASSANDRA_SHARED_LIBADD)
  PHP_NEW_EXTENSION(cassandra, php_cassandra.c, $ext_shared)
  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(cassandra, mbstring)
    PHP_ADD_EXTENSION_DEP(cassandra, bcmath)
  ])
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
    AC_MSG_ERROR(Cannot find libcassandra)
  fi

  if test -d "$CPP_DRIVER_DIR/lib64"; then
    CPP_DRIVER_LIBDIR="$CPP_DRIVER_DIR/lib64"
  else
    CPP_DRIVER_LIBDIR="$CPP_DRIVER_DIR/lib"
  fi

  case $CPP_DRIVER_DIR in
  /usr) ac_extra= ;;
  *)    ac_extra=-L$CPP_DRIVER_LIBDIR ;;
  esac

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

  PHP_ADD_LIBPATH($CPP_DRIVER_LIBDIR, CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(cassandra,, CASSANDRA_SHARED_LIBADD)
  PHP_ADD_INCLUDE($CPP_DRIVER_DIR/include)
fi
