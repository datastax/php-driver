#include "php_cassandra.h"
#include "util/future.h"

zend_class_entry *cassandra_default_cluster_ce = NULL;

ZEND_EXTERN_MODULE_GLOBALS(cassandra)

PHP_METHOD(DefaultCluster, connect)
{
  CassFuture* future = NULL;
  char* hash_key;
  int   hash_key_len;
  char* keyspace = NULL;
  int   keyspace_len;
  zval* timeout = NULL;
  cassandra_psession* psession;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sz", &keyspace, &keyspace_len, &timeout) == FAILURE) {
    return;
  }

  cassandra_cluster* cluster =
    (cassandra_cluster*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_default_session_ce);
  cassandra_session* session =
    (cassandra_session*) zend_object_store_get_object(return_value TSRMLS_CC);

  session->default_consistency = cluster->default_consistency;
  session->default_page_size   = cluster->default_page_size;
  session->default_timeout     = cluster->default_timeout;
  session->persist             = cluster->persist;

  if (session->persist) {
    zend_rsrc_list_entry *le;

    hash_key_len = spprintf(&hash_key, 0, "%s:session:%s",
                            cluster->hash_key, SAFE_STR(keyspace));

    if (zend_hash_find(&EG(persistent_list), hash_key, hash_key_len + 1, (void **)&le) == SUCCESS &&
        Z_TYPE_P(le) == php_le_cassandra_session()) {
      psession = (cassandra_psession*) le->ptr;
      session->session = psession->session;
      future = psession->future;
    }
  }

  if (future == NULL) {
    session->session = cass_session_new();

    if (keyspace)
      future = cass_session_connect_keyspace(session->session, cluster->cluster, keyspace);
    else
      future = cass_session_connect(session->session, cluster->cluster);

    if (session->persist) {
      zend_rsrc_list_entry pe;
      psession = (cassandra_psession*) pecalloc(1, sizeof(cassandra_psession), 1);
      psession->session = session->session;
      psession->future  = future;

      pe.type = php_le_cassandra_session();
      pe.ptr  = psession;

      zend_hash_update(&EG(persistent_list), hash_key, hash_key_len + 1, &pe, sizeof(zend_rsrc_list_entry), NULL);
      CASSANDRA_G(persistent_sessions)++;
    }
  }

  if (php_cassandra_future_wait_timed(future, timeout TSRMLS_CC) == FAILURE) {
    if (session->persist) {
      efree(hash_key);
    } else {
      cass_future_free(future);
    }

    return;
  }

  CassError rc = cass_future_error_code(future);

  if (rc != CASS_OK) {
    const char* message;
    size_t message_len;
    cass_future_error_message(future, &message, &message_len);
    zend_throw_exception_ex(exception_class(rc), rc TSRMLS_CC,
      "%.*s", (int) message_len, message);

    if (session->persist) {
      if (zend_hash_del(&EG(persistent_list), hash_key, hash_key_len + 1) == SUCCESS) {
        session->session = NULL;
      }

      efree(hash_key);
    } else {
      cass_future_free(future);
    }

    return;
  }

  if (session->persist)
    efree(hash_key);
}

PHP_METHOD(DefaultCluster, connectAsync)
{
  char* hash_key;
  int   hash_key_len;
  char* keyspace = NULL;
  int   keyspace_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &keyspace, &keyspace_len) == FAILURE) {
    return;
  }

  cassandra_cluster* cluster =
    (cassandra_cluster*) zend_object_store_get_object(getThis() TSRMLS_CC);

  object_init_ex(return_value, cassandra_future_session_ce);
  cassandra_future_session* future =
    (cassandra_future_session*) zend_object_store_get_object(return_value TSRMLS_CC);

  future->persist = cluster->persist;

  if (cluster->persist) {
    zend_rsrc_list_entry *le;

    hash_key_len = spprintf(&hash_key, 0,
      "%s:session:%s", cluster->hash_key, SAFE_STR(keyspace));

    future->hash_key     = hash_key;
    future->hash_key_len = hash_key_len;

    if (zend_hash_find(&EG(persistent_list), hash_key, hash_key_len + 1, (void **)&le) == SUCCESS) {
      if (Z_TYPE_P(le) == php_le_cassandra_session()) {
        cassandra_psession* psession = (cassandra_psession*) le->ptr;
        future->session = psession->session;
        future->future  = psession->future;
        return;
      }
    }
  }

  future->session = cass_session_new();

  if (keyspace) {
    future->future = cass_session_connect_keyspace(future->session, cluster->cluster, keyspace);
  } else {
    future->future = cass_session_connect(future->session, cluster->cluster);
  }

  if (cluster->persist) {
    cassandra_psession* psession =
      (cassandra_psession*) pecalloc(1, sizeof(cassandra_psession), 1);
    psession->session = future->session;
    psession->future  = future->future;

    zend_rsrc_list_entry le;
    le.type = php_le_cassandra_session();
    le.ptr  = psession;

    zend_hash_update(&EG(persistent_list), hash_key, hash_key_len + 1, &le, sizeof(zend_rsrc_list_entry), NULL);
    CASSANDRA_G(persistent_sessions)++;
  }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_connect, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
  ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_connectAsync, 0, ZEND_RETURN_VALUE, 0)
  ZEND_ARG_INFO(0, keyspace)
ZEND_END_ARG_INFO()

static zend_function_entry cassandra_default_cluster_methods[] = {
  PHP_ME(DefaultCluster, connect, arginfo_connect, ZEND_ACC_PUBLIC)
  PHP_ME(DefaultCluster, connectAsync, arginfo_connectAsync, ZEND_ACC_PUBLIC)
  PHP_FE_END
};

static zend_object_handlers cassandra_default_cluster_handlers;

static HashTable*
php_cassandra_default_cluster_properties(zval *object TSRMLS_DC)
{
  cassandra_cluster* cluster = (cassandra_cluster*) zend_object_store_get_object(object TSRMLS_CC);
  HashTable*         props   = zend_std_get_properties(object TSRMLS_CC);

  return props;
}

static int
php_cassandra_default_cluster_compare(zval *obj1, zval *obj2 TSRMLS_DC)
{
  if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
    return 1; /* different classes */

  return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void
php_cassandra_default_cluster_free(void *object TSRMLS_DC)
{
  cassandra_cluster* cluster = (cassandra_cluster*) object;

  zend_object_std_dtor(&cluster->zval TSRMLS_CC);

  if (cluster->persist) {
    efree(cluster->hash_key);
  } else {
    cass_cluster_free(cluster->cluster);
  }

  efree(cluster);
}

static zend_object_value
php_cassandra_default_cluster_new(zend_class_entry* class_type TSRMLS_DC)
{
  zend_object_value retval;
  cassandra_cluster *cluster;

  cluster = (cassandra_cluster*) ecalloc(1, sizeof(cassandra_cluster));

  zend_object_std_init(&cluster->zval, class_type TSRMLS_CC);
  object_properties_init(&cluster->zval, class_type);

  cluster->cluster             = NULL;
  cluster->default_consistency = CASS_CONSISTENCY_ONE;
  cluster->default_page_size   = 10000;
  cluster->default_timeout     = NULL;
  cluster->persist             = 0;
  cluster->hash_key            = NULL;

  retval.handle   = zend_objects_store_put(cluster,
                      (zend_objects_store_dtor_t) zend_objects_destroy_object,
                      php_cassandra_default_cluster_free, NULL TSRMLS_CC);
  retval.handlers = &cassandra_default_cluster_handlers;

  return retval;
}

void cassandra_define_DefaultCluster(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Cassandra\\DefaultCluster", cassandra_default_cluster_methods);
  cassandra_default_cluster_ce = zend_register_internal_class(&ce TSRMLS_CC);
  zend_class_implements(cassandra_default_cluster_ce TSRMLS_CC, 1, cassandra_cluster_ce);
  cassandra_default_cluster_ce->ce_flags     |= ZEND_ACC_FINAL_CLASS;
  cassandra_default_cluster_ce->create_object = php_cassandra_default_cluster_new;

  memcpy(&cassandra_default_cluster_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  cassandra_default_cluster_handlers.get_properties  = php_cassandra_default_cluster_properties;
  cassandra_default_cluster_handlers.compare_objects = php_cassandra_default_cluster_compare;
}
