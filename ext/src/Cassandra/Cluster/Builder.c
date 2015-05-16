#include "php_cassandra.h"
#include "util/consistency.h"
#include <ext/standard/php_smart_str.h>

zend_class_entry *cassandra_cluster_builder_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(ClusterBuilder, build)
{
  char* hash_key;
  int   hash_key_len;
  cassandra_cluster* cluster = NULL;

  cassandra_cluster_builder* builder =
    (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_default_cluster_ce);
  cluster = (cassandra_cluster*) zend_object_store_get_object(return_value TSRMLS_CC);

  cluster->persist                 = builder->persist;
  cluster->default_consistency     = builder->default_consistency;
  cluster->default_page_size       = builder->default_page_size;
  cluster->default_timeout         = builder->default_timeout;

  if (builder->persist) {
    zend_rsrc_list_entry *le;

    hash_key_len = spprintf(&hash_key, 0,
      "cassandra:%s:%d:%d:%s:%d:%d:%d:%s:%s:%d:%d", builder->contact_points,
      builder->port, builder->load_balancing_policy, SAFE_STR(builder->local_dc),
      builder->used_hosts_per_remote_dc, builder->allow_remote_dcs_for_local_cl,
      builder->use_token_aware_routing, SAFE_STR(builder->username),
      SAFE_STR(builder->password), builder->connect_timeout,
      builder->request_timeout);

    cluster->hash_key     = hash_key;
    cluster->hash_key_len = hash_key_len;

    if (zend_hash_find(&EG(persistent_list), hash_key, hash_key_len + 1, (void **)&le) == SUCCESS) {
      if (Z_TYPE_P(le) == php_le_cassandra_cluster()) {
        cluster->cluster = (CassCluster*) le->ptr;
        return;
      }
    }
  }

  cluster->cluster = cass_cluster_new();

  if (builder->load_balancing_policy == LOAD_BALANCING_ROUND_ROBIN) {
    cass_cluster_set_load_balance_round_robin(cluster->cluster);
  }

  if (builder->load_balancing_policy == LOAD_BALANCING_DC_AWARE_ROUND_ROBIN) {
    ASSERT_SUCCESS(cass_cluster_set_load_balance_dc_aware(cluster->cluster, builder->local_dc,
      builder->used_hosts_per_remote_dc, builder->allow_remote_dcs_for_local_cl));
  }

  cass_cluster_set_token_aware_routing(cluster->cluster, builder->use_token_aware_routing);

  if (builder->username) {
    cass_cluster_set_credentials(cluster->cluster, builder->username, builder->password);
  }

  cass_cluster_set_connect_timeout(cluster->cluster, builder->connect_timeout);
  cass_cluster_set_request_timeout(cluster->cluster, builder->request_timeout);

  if (builder->ssl_options) {
    cassandra_ssl* options = (cassandra_ssl*) zend_object_store_get_object(builder->ssl_options TSRMLS_CC);
    cass_cluster_set_ssl(cluster->cluster, options->ssl);
  }

  ASSERT_SUCCESS(cass_cluster_set_contact_points(cluster->cluster, builder->contact_points));
  ASSERT_SUCCESS(cass_cluster_set_port(cluster->cluster, builder->port));

  if (builder->persist) {
    zend_rsrc_list_entry le;
    le.type = php_le_cassandra_cluster();
    le.ptr  = cluster->cluster;

    if (zend_hash_update(&EG(persistent_list), hash_key, hash_key_len + 1, &le, sizeof(zend_rsrc_list_entry), NULL) == SUCCESS) {
      CASSANDRA_G(persistent_clusters)++;
    }
  }
}

PHP_METHOD(ClusterBuilder, withDefaultConsistency)
{
  zval* consistency;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &consistency) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (php_cassandra_get_consistency(consistency, &builder->default_consistency TSRMLS_CC) == FAILURE) {
    return;
  }

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withDefaultPageSize)
{
  zval* pageSize;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &pageSize) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (Z_TYPE_P(pageSize) == IS_NULL) {
    builder->default_page_size = -1;
  } else if (Z_TYPE_P(pageSize) == IS_LONG && Z_LVAL_P(pageSize) > 0) {
    builder->default_page_size = Z_LVAL_P(pageSize);
  } else {
    INVALID_ARGUMENT(pageSize, "a positive integer or null");
  }

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withDefaultTimeout)
{
  zval* timeout;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &timeout) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (Z_TYPE_P(timeout) == IS_NULL) {
    if (builder->default_timeout)
      zval_ptr_dtor(&builder->default_timeout);
    builder->default_timeout = NULL;
  } else if ((Z_TYPE_P(timeout) == IS_LONG && Z_LVAL_P(timeout) > 0) ||
             (Z_TYPE_P(timeout) == IS_DOUBLE && Z_DVAL_P(timeout) > 0)) {
    if (builder->default_timeout)
      zval_ptr_dtor(&builder->default_timeout);
    Z_ADDREF_P(timeout);
    builder->default_timeout = timeout;
  } else {
    INVALID_ARGUMENT(timeout, "a number of seconds greater than zero or null");
  }

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withContactPoints)
{
  zval***   args;
  zval*     host;
  int       argc, i;
  smart_str contactPoints = {NULL, 0, 0};
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
    return;
  }

  for (i = 0; i < argc; i++) {
    host = *args[i];

    if (Z_TYPE_P(host) != IS_STRING) {
      smart_str_free(&contactPoints);
      INVALID_ARGUMENT(host, "a string ip address or hostname");
    }

    if (i > 0) {
      smart_str_appendl(&contactPoints, ",", 1);
    }

    smart_str_appendl(&contactPoints, Z_STRVAL_P(host), Z_STRLEN_P(host));
  }

  smart_str_0(&contactPoints);

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  efree(builder->contact_points);
  builder->contact_points = contactPoints.c;

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withPort)
{
  zval* port;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &port) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (Z_TYPE_P(port) == IS_LONG && Z_LVAL_P(port) > 0 && Z_LVAL_P(port) < 65536) {
    builder->port = Z_LVAL_P(port);
  } else {
    INVALID_ARGUMENT(port, "an integer between 1 and 65535");
  }

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withRoundRobinLoadBalancingPolicy)
{
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters_none() == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (builder->local_dc) {
    efree(builder->local_dc);
    builder->local_dc = NULL;
  }

  builder->load_balancing_policy = LOAD_BALANCING_ROUND_ROBIN;

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withDatacenterAwareRoundRobinLoadBalancingPolicy)
{
  char*        local_dc;
  int          local_dc_len;
  zval*        hostPerRemoteDatacenter;
  zend_bool    allow_remote_dcs_for_local_cl;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szb", &local_dc, &local_dc_len, &hostPerRemoteDatacenter, &allow_remote_dcs_for_local_cl) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(hostPerRemoteDatacenter) != IS_LONG || Z_LVAL_P(hostPerRemoteDatacenter) < 0) {
    INVALID_ARGUMENT(hostPerRemoteDatacenter, "a positive integer");
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (builder->local_dc) {
    efree(builder->local_dc);
    builder->local_dc = NULL;
  }

  builder->load_balancing_policy         = LOAD_BALANCING_DC_AWARE_ROUND_ROBIN;
  builder->local_dc                      = estrndup(local_dc, local_dc_len);
  builder->used_hosts_per_remote_dc      = Z_LVAL_P(hostPerRemoteDatacenter);
  builder->allow_remote_dcs_for_local_cl = allow_remote_dcs_for_local_cl;

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withTokenAwareRouting)
{
  zend_bool enabled = 1;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &enabled) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  builder->use_token_aware_routing = enabled;

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withCredentials)
{
  zval* username;
  zval* password;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &username, &password) == FAILURE) {
    return;
  }

  if (Z_TYPE_P(username) != IS_STRING) {
    INVALID_ARGUMENT(username, "a string");
  }

  if (Z_TYPE_P(password) != IS_STRING) {
    INVALID_ARGUMENT(password, "a string");
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (builder->username) {
    efree(builder->username);
    efree(builder->password);
  }

  builder->username = estrndup(Z_STRVAL_P(username), Z_STRLEN_P(username));
  builder->password = estrndup(Z_STRVAL_P(password), Z_STRLEN_P(password));

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withConnectTimeout)
{
  double timeout;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &timeout) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  builder->connect_timeout = ceil(timeout * 1000);

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withRequestTimeout)
{
  double timeout;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &timeout) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  builder->request_timeout = ceil(timeout * 1000);

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withSSL)
{
  zval *ssl_options;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &ssl_options, cassandra_ssl_ce) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  if (builder->ssl_options)
    zval_ptr_dtor(&builder->ssl_options);

  Z_ADDREF_P(ssl_options);
  builder->ssl_options = ssl_options;

  RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ClusterBuilder, withPersistentSessions)
{
  zend_bool enabled = 1;
  cassandra_cluster_builder* builder = NULL;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &enabled) == FAILURE) {
    return;
  }

  builder = (cassandra_cluster_builder*) zend_object_store_get_object(getThis() TSRMLS_CC);

  builder->persist = enabled;

  RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_consistency, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, consistency)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_page_size, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, pageSize)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_contact_points, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, host)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_port, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dc_aware, 0, ZEND_RETURN_VALUE, 3)
  ZEND_ARG_INFO(0, localDatacenter)
  ZEND_ARG_INFO(0, hostPerRemoteDatacenter)
  ZEND_ARG_INFO(0, useRemoteDatacenterForLocalConsistencies)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_enabled, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, enabled)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_credentials, 0, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, username)
  ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_timeout, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ssl, 0, ZEND_RETURN_VALUE, 1)
  ZEND_ARG_OBJ_INFO(0, options, Cassandra\\SSLOptions, 0)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_cluster_builder_methods[] = {
  PHP_ME(ClusterBuilder, build, arginfo_none, ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withDefaultConsistency, arginfo_consistency,
    ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withDefaultPageSize, arginfo_page_size,
    ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withDefaultTimeout, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withContactPoints, arginfo_contact_points,
    ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withPort, arginfo_port, ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withRoundRobinLoadBalancingPolicy, arginfo_none,
    ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withDatacenterAwareRoundRobinLoadBalancingPolicy,
    arginfo_dc_aware, ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withTokenAwareRouting, arginfo_enabled,
    ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withCredentials, arginfo_credentials, ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withConnectTimeout, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withRequestTimeout, arginfo_timeout, ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withSSL, arginfo_ssl, ZEND_ACC_PUBLIC)
  PHP_ME(ClusterBuilder, withPersistentSessions, arginfo_enabled,
    ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_cluster_builder_handlers;

static HashTable*
php_cassandra_cluster_builder_properties(zval *object TSRMLS_DC)
{
  cassandra_cluster_builder* builder = (cassandra_cluster_builder*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*        props  = zend_std_get_properties(object TSRMLS_CC);

  zval* contactPoints;
  zval* loadBalancingPolicy;
  zval* localDatacenter;
  zval* hostPerRemoteDatacenter;
  zval* useRemoteDatacenterForLocalConsistencies;
  zval* useTokenAwareRouting;
  zval* username;
  zval* password;
  zval* connectTimeout;
  zval* requestTimeout;
  zval* sslOptions;
  zval* defaultConsistency;
  zval* defaultPageSize;
  zval* defaultTimeout;
  zval* usePersistentSessions;

  MAKE_STD_ZVAL(contactPoints);
  ZVAL_STRING(contactPoints, builder->contact_points, 1);

  MAKE_STD_ZVAL(loadBalancingPolicy);
  ZVAL_DOUBLE(loadBalancingPolicy, builder->load_balancing_policy);

  MAKE_STD_ZVAL(localDatacenter);
  MAKE_STD_ZVAL(hostPerRemoteDatacenter);
  MAKE_STD_ZVAL(useRemoteDatacenterForLocalConsistencies);
  if (builder->load_balancing_policy == LOAD_BALANCING_DC_AWARE_ROUND_ROBIN) {
    ZVAL_STRING(localDatacenter, builder->local_dc, 1);
    ZVAL_LONG(hostPerRemoteDatacenter, builder->used_hosts_per_remote_dc);
    ZVAL_BOOL(useRemoteDatacenterForLocalConsistencies, builder->allow_remote_dcs_for_local_cl);
  } else {
    ZVAL_NULL(localDatacenter);
    ZVAL_NULL(hostPerRemoteDatacenter);
    ZVAL_NULL(useRemoteDatacenterForLocalConsistencies);
  }

  MAKE_STD_ZVAL(useTokenAwareRouting);
  ZVAL_BOOL(useTokenAwareRouting, builder->use_token_aware_routing);

  MAKE_STD_ZVAL(username);
  MAKE_STD_ZVAL(password);
  if (builder->username) {
    ZVAL_STRING(username, builder->username, 1);
    ZVAL_STRING(password, builder->password, 1);
  } else {
    ZVAL_NULL(username);
    ZVAL_NULL(password);
  }

  MAKE_STD_ZVAL(connectTimeout);
  ZVAL_DOUBLE(connectTimeout, (double) builder->connect_timeout / 1000);
  MAKE_STD_ZVAL(requestTimeout);
  ZVAL_DOUBLE(requestTimeout, (double) builder->request_timeout / 1000);

  MAKE_STD_ZVAL(sslOptions);
  if (builder->ssl_options) {
    ZVAL_ZVAL(sslOptions, builder->ssl_options, 1, 0);
  } else {
    ZVAL_NULL(sslOptions);
  }

  MAKE_STD_ZVAL(defaultConsistency);
  ZVAL_LONG(defaultConsistency, builder->default_consistency);
  MAKE_STD_ZVAL(defaultPageSize);
  ZVAL_LONG(defaultPageSize, builder->default_page_size);
  MAKE_STD_ZVAL(defaultTimeout);
  if (builder->default_timeout) {
    ZVAL_LONG(defaultTimeout, Z_LVAL_P(builder->default_timeout));
  } else {
    ZVAL_NULL(defaultTimeout);
  }

  MAKE_STD_ZVAL(usePersistentSessions);
  ZVAL_BOOL(usePersistentSessions, builder->persist);

  zend_hash_update(props, "contactPoints", sizeof("contactPoints"),
    &contactPoints, sizeof(zval), NULL);
  zend_hash_update(props, "loadBalancingPolicy", sizeof("loadBalancingPolicy"),
    &loadBalancingPolicy, sizeof(zval), NULL);
  zend_hash_update(props, "localDatacenter", sizeof("localDatacenter"),
    &localDatacenter, sizeof(zval), NULL);
  zend_hash_update(props, "hostPerRemoteDatacenter", sizeof("hostPerRemoteDatacenter"),
    &hostPerRemoteDatacenter, sizeof(zval), NULL);
  zend_hash_update(props, "useRemoteDatacenterForLocalConsistencies", sizeof("useRemoteDatacenterForLocalConsistencies"),
    &useRemoteDatacenterForLocalConsistencies, sizeof(zval), NULL);
  zend_hash_update(props, "useTokenAwareRouting", sizeof("useTokenAwareRouting"),
    &useTokenAwareRouting, sizeof(zval), NULL);
  zend_hash_update(props, "username", sizeof("username"), &username, sizeof(zval), NULL);
  zend_hash_update(props, "password", sizeof("password"), &password, sizeof(zval), NULL);
  zend_hash_update(props, "connectTimeout", sizeof("connectTimeout"),
    &connectTimeout, sizeof(zval), NULL);
  zend_hash_update(props, "requestTimeout", sizeof("requestTimeout"),
    &requestTimeout, sizeof(zval), NULL);
  zend_hash_update(props, "sslOptions", sizeof("sslOptions"),
    &sslOptions, sizeof(zval), NULL);
  zend_hash_update(props, "defaultConsistency", sizeof("defaultConsistency"),
    &defaultConsistency, sizeof(zval), NULL);
  zend_hash_update(props, "defaultPageSize", sizeof("defaultPageSize"),
    &defaultPageSize, sizeof(zval), NULL);
  zend_hash_update(props, "defaultTimeout", sizeof("defaultTimeout"),
    &defaultTimeout, sizeof(zval), NULL);
  zend_hash_update(props, "usePersistentSessions", sizeof("usePersistentSessions"),
    &usePersistentSessions, sizeof(zval), NULL);

  return props;
}

static int
php_cassandra_cluster_builder_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_cluster_builder_free(void *object TSRMLS_DC)
{
  cassandra_cluster_builder* builder = (cassandra_cluster_builder*) object;

  zend_object_std_dtor(&builder->zval TSRMLS_CC);
  efree(builder->contact_points);

  if (builder->local_dc)
    efree(builder->local_dc);

  if (builder->username)
    efree(builder->username);

  if (builder->password)
    efree(builder->password);

  if (builder->ssl_options)
    zval_ptr_dtor(&builder->ssl_options);

  if (builder->default_timeout)
    zval_ptr_dtor(&builder->default_timeout);

  efree(builder);
}

static zend_object_value
php_cassandra_cluster_builder_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_cluster_builder *builder;

  builder = (cassandra_cluster_builder*) ecalloc(1, sizeof(cassandra_cluster_builder));

  zend_object_std_init(&builder->zval, class_type TSRMLS_CC);
  object_properties_init(&builder->zval, class_type);

  builder->contact_points = estrdup("127.0.0.1");
  builder->port = 9042;
  builder->load_balancing_policy = LOAD_BALANCING_ROUND_ROBIN;
  builder->local_dc = NULL;
  builder->used_hosts_per_remote_dc = 0;
  builder->allow_remote_dcs_for_local_cl = 0;
  builder->use_token_aware_routing = 1;
  builder->username = NULL;
  builder->password = NULL;
  builder->connect_timeout = 5000;
  builder->request_timeout = 12000;
  builder->ssl_options = NULL;
  builder->default_consistency = CASS_CONSISTENCY_ONE;
  builder->default_page_size = 10000;
  builder->default_timeout = NULL;
  builder->persist = 1;

  retval.handle   = zend_objects_store_put(builder,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_cluster_builder_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_cluster_builder_handlers;

  return retval;
}

void cassandra_define_ClusterBuilder(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\Cluster\\Builder", cassandra_cluster_builder_methods);
  cassandra_cluster_builder_ce = zend_register_internal_class(&ce TSRMLS_CC);
  cassandra_cluster_builder_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_cluster_builder_ce->create_object = php_cassandra_cluster_builder_new;

  memcpy(&cassandra_cluster_builder_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_cluster_builder_handlers.get_properties  = php_cassandra_cluster_builder_properties;
  cassandra_cluster_builder_handlers.compare_objects = php_cassandra_cluster_builder_compare;
}
