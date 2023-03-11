#include <php.h>

#include <php_driver.h>
#include <php_driver_types.h>

#include "BuilderHandlers.h"

BEGIN_EXTERN_C()
static zend_object_handlers php_driver_cluster_builder_handlers;

static HashTable *php_driver_cluster_builder_gc(zend_object *object, zval **table, int *n)
{
    *table = NULL;
    *n = 0;
    return zend_std_get_properties(object);
}

static HashTable *php_driver_cluster_builder_properties(zend_object *object)
{
    zval contactPoints;
    zval loadBalancingPolicy;
    zval localDatacenter;
    zval hostPerRemoteDatacenter;
    zval useRemoteDatacenterForLocalConsistencies;
    zval useTokenAwareRouting;
    zval username;
    zval password;
    zval connectTimeout;
    zval requestTimeout;
    zval sslOptions;
    zval defaultConsistency;
    zval defaultPageSize;
    zval defaultTimeout;
    zval usePersistentSessions;
    zval protocolVersion;
    zval ioThreads;
    zval coreConnectionPerHost;
    zval maxConnectionsPerHost;
    zval reconnectInterval;
    zval latencyAwareRouting;
    zval tcpNodelay;
    zval tcpKeepalive;
    zval retryPolicy;
    zval blacklistHosts;
    zval whitelistHosts;
    zval blacklistDCs;
    zval whitelistDCs;
    zval timestampGen;
    zval schemaMetadata;
    zval hostnameResolution;
    zval randomizedContactPoints;
    zval connectionHeartbeatInterval;

    php_driver_cluster_builder *self = php_driver_cluster_builder_object_fetch(object);
    HashTable *props = zend_std_get_properties(object);

    ZVAL_STRING(&contactPoints, self->contact_points);

    ZVAL_LONG(&loadBalancingPolicy, self->load_balancing_policy);

    if (self->load_balancing_policy == LOAD_BALANCING_DC_AWARE_ROUND_ROBIN)
    {
        ZVAL_STRING(&localDatacenter, self->local_dc);
        ZVAL_LONG(&hostPerRemoteDatacenter, self->used_hosts_per_remote_dc);
        ZVAL_BOOL(&useRemoteDatacenterForLocalConsistencies, self->allow_remote_dcs_for_local_cl);
    }
    else
    {
        ZVAL_NULL(&localDatacenter);
        ZVAL_NULL(&hostPerRemoteDatacenter);
        ZVAL_NULL(&useRemoteDatacenterForLocalConsistencies);
    }

    ZVAL_BOOL(&useTokenAwareRouting, self->use_token_aware_routing);

    if (self->username)
    {
        ZVAL_STRING(&username, self->username);
        ZVAL_STRING(&password, self->password);
    }
    else
    {
        ZVAL_NULL(&username);
        ZVAL_NULL(&password);
    }

    ZVAL_DOUBLE(&connectTimeout, (double)self->connect_timeout / 1000);
    ZVAL_DOUBLE(&requestTimeout, (double)self->request_timeout / 1000);
    if (!Z_ISUNDEF(self->ssl_options))
    {
        ZVAL_COPY(&sslOptions, &self->ssl_options);
    }
    else
    {
        ZVAL_NULL(&sslOptions);
    }

    ZVAL_LONG(&defaultConsistency, self->default_consistency);
    ZVAL_LONG(&defaultPageSize, self->default_page_size);
    if (!Z_ISUNDEF(self->default_timeout))
    {
        ZVAL_LONG(&defaultTimeout, PHP5TO7_Z_LVAL_MAYBE_P(self->default_timeout));
    }
    else
    {
        ZVAL_NULL(&defaultTimeout);
    }

    ZVAL_BOOL(&usePersistentSessions, self->persist);
    ZVAL_LONG(&protocolVersion, self->protocol_version);
    ZVAL_LONG(&ioThreads, self->io_threads);
    ZVAL_LONG(&coreConnectionPerHost, self->core_connections_per_host);
    ZVAL_LONG(&maxConnectionsPerHost, self->max_connections_per_host);
    ZVAL_DOUBLE(&reconnectInterval, (double)self->reconnect_interval / 1000);
    ZVAL_BOOL(&latencyAwareRouting, self->enable_latency_aware_routing);
    ZVAL_BOOL(&tcpNodelay, self->enable_tcp_nodelay);

    if (self->enable_tcp_keepalive)
    {
        ZVAL_DOUBLE(&tcpKeepalive, (double)self->tcp_keepalive_delay / 1000);
    }
    else
    {
        ZVAL_NULL(&tcpKeepalive);
    }

    if (!Z_ISUNDEF(self->retry_policy))
    {
        ZVAL_COPY(&retryPolicy, &self->retry_policy);
    }
    else
    {
        ZVAL_NULL(&retryPolicy);
    }

    if (self->blacklist_hosts != NULL)
    {
        ZVAL_STR(&blacklistHosts, self->blacklist_hosts);
    }
    else
    {
        ZVAL_NULL(&blacklistHosts);
    }

    if (self->whitelist_hosts)
    {
        ZVAL_STR(&whitelistHosts, self->whitelist_hosts);
    }
    else
    {
        ZVAL_NULL(&whitelistHosts);
    }

    if (self->blacklist_dcs)
    {
        ZVAL_STRING(&blacklistDCs, self->blacklist_dcs);
    }
    else
    {
        ZVAL_NULL(&blacklistDCs);
    }

    if (self->whitelist_dcs)
    {
        ZVAL_STRING(&whitelistDCs, self->whitelist_dcs);
    }
    else
    {
        ZVAL_NULL(&whitelistDCs);
    }

    if (self->timestamp_gen != NULL)
    {
        ZVAL_OBJ_COPY(&timestampGen, &self->timestamp_gen->zval);
    }
    else
    {
        ZVAL_NULL(&timestampGen);
    }

    ZVAL_BOOL(&schemaMetadata, self->enable_schema);

    ZVAL_BOOL(&hostnameResolution, self->enable_hostname_resolution);

    ZVAL_BOOL(&randomizedContactPoints, self->enable_randomized_contact_points);

    ZVAL_LONG(&connectionHeartbeatInterval, self->connection_heartbeat_interval);

    PHP5TO7_ZEND_HASH_UPDATE(props, "contactPoints", sizeof("contactPoints"), &contactPoints, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "loadBalancingPolicy", sizeof("loadBalancingPolicy"), &loadBalancingPolicy,
                             sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "localDatacenter", sizeof("localDatacenter"), &localDatacenter, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "hostPerRemoteDatacenter", sizeof("hostPerRemoteDatacenter"),
                             &hostPerRemoteDatacenter, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "useRemoteDatacenterForLocalConsistencies",
                             sizeof("useRemoteDatacenterForLocalConsistencies"),
                             &useRemoteDatacenterForLocalConsistencies, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "useTokenAwareRouting", sizeof("useTokenAwareRouting"), &useTokenAwareRouting,
                             sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "username", sizeof("username"), &username, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "password", sizeof("password"), &password, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "connectTimeout", sizeof("connectTimeout"), &connectTimeout, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "requestTimeout", sizeof("requestTimeout"), &requestTimeout, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "sslOptions", sizeof("sslOptions"), &sslOptions, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "defaultConsistency", sizeof("defaultConsistency"), &defaultConsistency,
                             sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "defaultPageSize", sizeof("defaultPageSize"), &defaultPageSize, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "defaultTimeout", sizeof("defaultTimeout"), &defaultTimeout, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "usePersistentSessions", sizeof("usePersistentSessions"), &usePersistentSessions,
                             sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "protocolVersion", sizeof("protocolVersion"), &protocolVersion, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "ioThreads", sizeof("ioThreads"), &ioThreads, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "coreConnectionPerHost", sizeof("coreConnectionPerHost"), &coreConnectionPerHost,
                             sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "maxConnectionsPerHost", sizeof("maxConnectionsPerHost"), &maxConnectionsPerHost,
                             sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "reconnectInterval", sizeof("reconnectInterval"), &reconnectInterval, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "latencyAwareRouting", sizeof("latencyAwareRouting"), &latencyAwareRouting,
                             sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "tcpNodelay", sizeof("tcpNodelay"), &tcpNodelay, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "tcpKeepalive", sizeof("tcpKeepalive"), &tcpKeepalive, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "retryPolicy", sizeof("retryPolicy"), &retryPolicy, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "timestampGenerator", sizeof("timestampGenerator"), &timestampGen, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "schemaMetadata", sizeof("schemaMetadata"), &schemaMetadata, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "blacklist_hosts", sizeof("blacklist_hosts"), &blacklistHosts, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "whitelist_hosts", sizeof("whitelist_hosts"), &whitelistHosts, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "blacklist_dcs", sizeof("blacklist_dcs"), &blacklistDCs, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "whitelist_dcs", sizeof("whitelist_dcs"), &whitelistDCs, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "hostnameResolution", sizeof("hostnameResolution"), &hostnameResolution,
                             sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "randomizedContactPoints", sizeof("randomizedContactPoints"),
                             &randomizedContactPoints, sizeof(zval));
    PHP5TO7_ZEND_HASH_UPDATE(props, "connectionHeartbeatInterval", sizeof("connectionHeartbeatInterval"),
                             &connectionHeartbeatInterval, sizeof(zval));

    return props;
}

static int php_driver_cluster_builder_compare(zval *obj1, zval *obj2)
{
    if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
        return 1; /* different classes */

    return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void php_driver_cluster_builder_free(zend_object *object)
{
    php_driver_cluster_builder *self = PHP5TO7_ZEND_OBJECT_GET(cluster_builder, object);

    efree(self->contact_points);
    self->contact_points = NULL;

    if (self->local_dc)
    {
        efree(self->local_dc);
        self->local_dc = NULL;
    }

    if (self->username)
    {
        efree(self->username);
        self->username = NULL;
    }

    if (self->password)
    {
        efree(self->password);
        self->password = NULL;
    }

    if (self->whitelist_hosts != NULL)
    {
        zend_string_release(self->whitelist_hosts);
        self->whitelist_hosts = NULL;
    }

    if (self->blacklist_hosts != NULL)
    {
        zend_string_release(self->blacklist_hosts);
        self->blacklist_hosts = NULL;
    }

    if (self->whitelist_dcs)
    {
        efree(self->whitelist_dcs);
        self->whitelist_dcs = NULL;
    }

    if (self->blacklist_dcs)
    {
        efree(self->blacklist_dcs);
        self->whitelist_dcs = NULL;
    }

    if (!Z_ISUNDEF(self->ssl_options))
    {
        zval_ptr_dtor(&self->ssl_options);
        ZVAL_UNDEF(&self->ssl_options);
    }

    if (!Z_ISUNDEF(self->default_timeout))
    {
        zval_ptr_dtor(&self->default_timeout);
        ZVAL_UNDEF(&self->default_timeout);
    }

    if (!Z_ISUNDEF(self->retry_policy))
    {
        zval_ptr_dtor(&self->retry_policy);
        ZVAL_UNDEF(&self->retry_policy);
    }

    if (self->timestamp_gen)
    {
        zend_object_release(&self->timestamp_gen->zval);
    }
}

php5to7_zend_object php_driver_cluster_builder_new(zend_class_entry *ce)
{
    php_driver_cluster_builder *self = emalloc(sizeof(php_driver_cluster_builder) + zend_object_properties_size(ce));

    self->contact_points = estrdup("127.0.0.1");
    self->port = 9042;
    self->load_balancing_policy = LOAD_BALANCING_DEFAULT;
    self->local_dc = NULL;
    self->used_hosts_per_remote_dc = 0;
    self->allow_remote_dcs_for_local_cl = 0;
    self->use_token_aware_routing = 1;
    self->username = NULL;
    self->password = NULL;
    self->connect_timeout = 5000;
    self->request_timeout = 12000;
    self->default_consistency = CASS_CONSISTENCY_LOCAL_ONE;
    self->default_page_size = 5000;
    self->persist = 1;
    self->protocol_version = 4;
    self->io_threads = 1;
    self->core_connections_per_host = 1;
    self->max_connections_per_host = 2;
    self->reconnect_interval = 2000;
    self->enable_latency_aware_routing = 1;
    self->enable_tcp_nodelay = 1;
    self->enable_tcp_keepalive = 0;
    self->tcp_keepalive_delay = 0;
    self->enable_schema = 1;
    self->blacklist_hosts = NULL;
    self->whitelist_hosts = NULL;
    self->blacklist_dcs = NULL;
    self->whitelist_dcs = NULL;
    self->enable_hostname_resolution = 0;
    self->enable_randomized_contact_points = 1;
    self->connection_heartbeat_interval = 30;
    self->timestamp_gen = NULL;

    ZVAL_UNDEF(&self->ssl_options);
    ZVAL_UNDEF(&self->default_timeout);
    ZVAL_UNDEF(&self->retry_policy);

    zend_object_std_init(&self->zval, ce);
    self->zval.handlers = &php_driver_cluster_builder_handlers;

    if (zend_object_properties_size(ce) > 0)
    {
        object_properties_init(&self->zval, ce);
    }

    return &self->zval;
}

void php_driver_initialize_cluster_builder_handlers()
{
    memcpy(&php_driver_cluster_builder_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    php_driver_cluster_builder_handlers.get_properties = php_driver_cluster_builder_properties;
    php_driver_cluster_builder_handlers.get_gc = php_driver_cluster_builder_gc;
    php_driver_cluster_builder_handlers.compare = php_driver_cluster_builder_compare;
    php_driver_cluster_builder_handlers.offset = XtOffsetOf(php_driver_cluster_builder, zval);
    php_driver_cluster_builder_handlers.free_obj = php_driver_cluster_builder_free;
}

END_EXTERN_C()
