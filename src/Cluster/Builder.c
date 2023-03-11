/**
 * Copyright 2015-2017 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zend_smart_str.h>

#include <cassandra.h>

#include "php_driver.h"
#include "php_driver_globals.h"
#include "php_driver_types.h"
#include "util/consistency.h"

#include "Builder.h"
#include "BuilderHandlers.h"
#include "Builder_arginfo.h"
#include "zend_portability.h"

BEGIN_EXTERN_C()
zend_class_entry *php_driver_cluster_builder_ce = NULL;

static zend_always_inline zend_string *php_driver_build_hosts_str(zval *args, size_t argc)
{
    smart_str hosts = {0};

    for (size_t i = 0; i < argc; i++)
    {
        zval *host = &args[i];

        if (Z_TYPE_P(host) != IS_STRING)
        {
            smart_str_free(&hosts);
            return NULL;
        }

        if (i > 0)
        {
            smart_str_appendl(&hosts, ",", 1);
        }

        smart_str_append(&hosts, Z_STR_P(host));
    }

    return smart_str_extract(&hosts);
}

static zend_always_inline void php_driver_parse_hosts(INTERNAL_FUNCTION_PARAMETERS, zend_string **out_hosts)
{
    zval *args = NULL;
    int argc = 0;

    ZEND_PARSE_PARAMETERS_START(1, -1)
    Z_PARAM_VARIADIC('+', args, argc)
    ZEND_PARSE_PARAMETERS_END();

    zend_string *hosts = php_driver_build_hosts_str(args, argc);

    if (hosts == NULL)
    {
        throw_invalid_argument(args, "hosts", "a string ip address or hostname");
        return;
    }

    if (*out_hosts != NULL)
    {
        zend_string_release(*out_hosts);
    }

    *out_hosts = hosts;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, build)
{
    CassError rc;
    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    object_init_ex(return_value, php_driver_default_cluster_ce);
    php_driver_cluster *cluster = PHP_DRIVER_GET_CLUSTER(return_value);

    cluster->persist = self->persist;
    cluster->default_consistency = self->default_consistency;
    cluster->default_page_size = self->default_page_size;

    ZVAL_COPY(&cluster->default_timeout, &self->default_timeout);

    if (self->persist)
    {
        cluster->hash_key_len = spprintf(
            &cluster->hash_key, 0,
            PHP_DRIVER_NAME ":%s:%d:%d:%s:%d:%d:%d:%s:%s:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%s:%s:%s:%s",
            ZSTR_VAL(self->contact_points), self->port, self->load_balancing_policy, SAFE_ZEND_STRING(self->local_dc),
            self->used_hosts_per_remote_dc, self->allow_remote_dcs_for_local_cl, self->use_token_aware_routing,
            SAFE_ZEND_STRING(self->username), SAFE_ZEND_STRING(self->password), self->connect_timeout,
            self->request_timeout, self->protocol_version, self->io_threads, self->core_connections_per_host,
            self->max_connections_per_host, self->reconnect_interval, self->enable_latency_aware_routing,
            self->enable_tcp_nodelay, self->enable_tcp_keepalive, self->tcp_keepalive_delay, self->enable_schema,
            self->enable_hostname_resolution, self->enable_randomized_contact_points,
            self->connection_heartbeat_interval, SAFE_ZEND_STRING(self->whitelist_hosts),
            SAFE_ZEND_STRING(self->whitelist_dcs), SAFE_ZEND_STRING(self->blacklist_hosts),
            SAFE_ZEND_STRING(self->blacklist_dcs));

        php5to7_zend_resource_le *le;

        if (PHP5TO7_ZEND_HASH_FIND(&EG(persistent_list), cluster->hash_key, cluster->hash_key_len + 1, le))
        {
            if (Z_TYPE_P(le) == php_le_php_driver_cluster())
            {
                cluster->cluster = (CassCluster *)Z_RES_P(le)->ptr;
                return; /* Return cached version */
            }
        }
    }

    cluster->cluster = cass_cluster_new();

    switch (self->load_balancing_policy)
    {
    case LOAD_BALANCING_DEFAULT:
        break;
    case LOAD_BALANCING_ROUND_ROBIN:
        cass_cluster_set_load_balance_round_robin(cluster->cluster);
        break;
    case LOAD_BALANCING_DC_AWARE_ROUND_ROBIN:
        ASSERT_SUCCESS(cass_cluster_set_load_balance_dc_aware(cluster->cluster, SAFE_ZEND_STRING(self->local_dc),
                                                              self->used_hosts_per_remote_dc,
                                                              self->allow_remote_dcs_for_local_cl));
        break;
    }

    if (self->blacklist_hosts != NULL)
    {
        cass_cluster_set_blacklist_filtering(cluster->cluster, ZSTR_VAL(self->blacklist_hosts));
    }

    if (self->whitelist_hosts != NULL)
    {
        cass_cluster_set_whitelist_filtering(cluster->cluster, ZSTR_VAL(self->whitelist_hosts));
    }

    if (self->blacklist_dcs != NULL)
    {
        cass_cluster_set_blacklist_dc_filtering(cluster->cluster, ZSTR_VAL(self->blacklist_dcs));
    }

    if (self->whitelist_dcs != NULL)
    {
        cass_cluster_set_whitelist_dc_filtering(cluster->cluster, ZSTR_VAL(self->whitelist_dcs));
    }

    cass_cluster_set_token_aware_routing(cluster->cluster, self->use_token_aware_routing);

    if (self->username != NULL)
    {
        cass_cluster_set_credentials(cluster->cluster, ZSTR_VAL(self->username), ZSTR_VAL(self->password));
    }

    cass_cluster_set_connect_timeout(cluster->cluster, self->connect_timeout);
    cass_cluster_set_request_timeout(cluster->cluster, self->request_timeout);

    if (!Z_ISUNDEF(self->ssl_options))
    {
        php_driver_ssl *options = PHP_DRIVER_GET_SSL(&self->ssl_options);
        cass_cluster_set_ssl(cluster->cluster, options->ssl);
    }

    ASSERT_SUCCESS(cass_cluster_set_contact_points(cluster->cluster, ZSTR_VAL(self->contact_points)));
    ASSERT_SUCCESS(cass_cluster_set_port(cluster->cluster, self->port));

    ASSERT_SUCCESS(cass_cluster_set_protocol_version(cluster->cluster, self->protocol_version));
    ASSERT_SUCCESS(cass_cluster_set_num_threads_io(cluster->cluster, self->io_threads));
    ASSERT_SUCCESS(cass_cluster_set_core_connections_per_host(cluster->cluster, self->core_connections_per_host));
    cass_cluster_set_constant_reconnect(cluster->cluster, self->reconnect_interval);
    cass_cluster_set_latency_aware_routing(cluster->cluster, self->enable_latency_aware_routing);
    cass_cluster_set_tcp_nodelay(cluster->cluster, self->enable_tcp_nodelay);
    cass_cluster_set_tcp_keepalive(cluster->cluster, self->enable_tcp_keepalive, self->tcp_keepalive_delay);
    cass_cluster_set_use_schema(cluster->cluster, self->enable_schema);

    rc = cass_cluster_set_use_hostname_resolution(cluster->cluster, self->enable_hostname_resolution);
    if (rc == CASS_ERROR_LIB_NOT_IMPLEMENTED && self->enable_hostname_resolution)
    {
        php_error_docref(NULL, E_WARNING,
                         "The underlying C/C++ driver does not implement hostname resolution it will be disabled");
    }
    else
    {
        ASSERT_SUCCESS(rc);
    }
    ASSERT_SUCCESS(
        cass_cluster_set_use_randomized_contact_points(cluster->cluster, self->enable_randomized_contact_points));
    cass_cluster_set_connection_heartbeat_interval(cluster->cluster, self->connection_heartbeat_interval);

    if (self->timestamp_gen != NULL)
    {
        php_driver_timestamp_gen *timestamp_gen = self->timestamp_gen;
        cass_cluster_set_timestamp_gen(cluster->cluster, timestamp_gen->gen);
    }

    if (self->retry_policy != NULL)
    {
        cass_cluster_set_retry_policy(cluster->cluster, self->retry_policy->policy);
    }

    if (self->persist)
    {
        php5to7_zend_resource_le resource;

        ZVAL_NEW_PERSISTENT_RES(&resource, 0, cluster->cluster, php_le_php_driver_cluster());

        PHP5TO7_ZEND_HASH_UPDATE(&EG(persistent_list), cluster->hash_key, cluster->hash_key_len + 1, &resource,
                                 sizeof(php5to7_zend_resource_le));
        PHP_DRIVER_G(persistent_clusters)++;
    }
}

ZEND_METHOD(Cassandra_Cluster_Builder, withDefaultConsistency)
{
    zend_long consistency;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(consistency)
    ZEND_PARSE_PARAMETERS_END();

    if (php_driver_validate_consistency((uint32_t)consistency) == -1)
    {
        zval consistency_val;
        ZVAL_LONG(&consistency_val, consistency);

        throw_invalid_argument(&consistency_val, "consistency", "one of " PHP_DRIVER_NAMESPACE "::CONSISTENCY_*");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->default_consistency = consistency;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withDefaultPageSize)
{
    zend_long pageSize = -1;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(pageSize)
    ZEND_PARSE_PARAMETERS_END();

    if (pageSize < 0)
    {
        zval val;
        ZVAL_LONG(&val, pageSize);
        throw_invalid_argument(&val, "pageSize", "a positive integer or null");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->default_page_size = (int)pageSize;
    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withDefaultTimeout)
{
    double timeout = 0.0;
    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END();

    zval val;
    ZVAL_DOUBLE(&val, timeout);

    if (timeout < 0.0)
    {
        throw_invalid_argument(&val, "timeout", "a positive number or null");
        return;
    }

    if (!Z_ISUNDEF(self->default_timeout))
    {
        zval_ptr_dtor(&self->default_timeout);
    }

    ZVAL_COPY_VALUE(&self->default_timeout, &val);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withContactPoints)
{
    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    php_driver_parse_hosts(INTERNAL_FUNCTION_PARAM_PASSTHRU, &self->contact_points);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withPort)
{
    zend_long port = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END();

    if (port < 1 || port > 65535)
    {
        zval val;
        ZVAL_LONG(&val, port);
        throw_invalid_argument(&val, "port", "an integer between 1 and 65535");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->port = (int)port;
    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withRoundRobinLoadBalancingPolicy)
{
    ZEND_PARSE_PARAMETERS_NONE();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (self->local_dc)
    {
        zend_string_release(self->local_dc);
        self->local_dc = NULL;
    }

    self->load_balancing_policy = LOAD_BALANCING_ROUND_ROBIN;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withDatacenterAwareRoundRobinLoadBalancingPolicy)
{
    zend_string *local_dc;
    zend_long hostPerRemoteDatacenter = 0;
    zend_bool allow_remote_dcs_for_local_cl;

    ZEND_PARSE_PARAMETERS_START(3, 3)
    Z_PARAM_STR(local_dc)
    Z_PARAM_LONG(hostPerRemoteDatacenter)
    Z_PARAM_BOOL(allow_remote_dcs_for_local_cl)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (hostPerRemoteDatacenter < 0)
    {
        zval val;
        ZVAL_LONG(&val, hostPerRemoteDatacenter);
        throw_invalid_argument(&val, "hostPerRemoteDatacenter", "a positive integer");
        return;
    }

    if (self->local_dc)
    {
        zend_string_release(self->local_dc);
        self->local_dc = NULL;
    }

    self->load_balancing_policy = LOAD_BALANCING_DC_AWARE_ROUND_ROBIN;
    self->local_dc = local_dc;
    self->used_hosts_per_remote_dc = hostPerRemoteDatacenter;
    self->allow_remote_dcs_for_local_cl = allow_remote_dcs_for_local_cl;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withBlackListHosts)
{
    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    php_driver_parse_hosts(INTERNAL_FUNCTION_PARAM_PASSTHRU, &self->blacklist_hosts);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withWhiteListHosts)
{
    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    php_driver_parse_hosts(INTERNAL_FUNCTION_PARAM_PASSTHRU, &self->whitelist_hosts);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withBlackListDCs)
{
    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    php_driver_parse_hosts(INTERNAL_FUNCTION_PARAM_PASSTHRU, &self->blacklist_dcs);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withWhiteListDCs)
{
    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    php_driver_parse_hosts(INTERNAL_FUNCTION_PARAM_PASSTHRU, &self->whitelist_dcs);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withTokenAwareRouting)
{
    zend_bool enabled = true;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(enabled)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->use_token_aware_routing = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withCredentials)
{
    zend_string *username = NULL;
    zend_string *password = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STR(username)
    Z_PARAM_STR(password)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (self->username != NULL && self->password != NULL)
    {
        zend_string_release(self->username);
        zend_string_release(self->password);
    }

    self->username = username;
    self->password = password;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withConnectTimeout)
{
    zend_long timeout = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(timeout)
    ZEND_PARSE_PARAMETERS_END();

    if (timeout < 1)
    {
        zval val;
        ZVAL_LONG(&val, timeout);
        throw_invalid_argument(&val, "timeout", "a positive number");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->connect_timeout = timeout * 1000;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withRequestTimeout)
{
    zend_long timeout = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(timeout)
    ZEND_PARSE_PARAMETERS_END();

    if (timeout < 1)
    {
        zval val;
        ZVAL_LONG(&val, timeout);
        throw_invalid_argument(&val, "timeout", "a positive number");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->request_timeout = timeout * 1000;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withSSL)
{
    zval *ssl_options = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &ssl_options, php_driver_ssl_ce) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (!Z_ISUNDEF(self->ssl_options))
        zval_ptr_dtor(&self->ssl_options);

    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(self->ssl_options), ssl_options);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withPersistentSessions)
{
    zend_bool enabled = true;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(enabled)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->persist = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withProtocolVersion)
{
    zend_long version;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(version)
    ZEND_PARSE_PARAMETERS_END();

    if (version < 1)
    {
        zval val;
        ZVAL_LONG(&val, version);
        throw_invalid_argument(&val, "version", "a positive number");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->protocol_version = version;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withIOThreads)
{
    zend_long count;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(count)
    ZEND_PARSE_PARAMETERS_END();

    if (count < 1 || count > 128)
    {
        zval val;
        ZVAL_LONG(&val, count);
        throw_invalid_argument(&val, "count", "a number between 1 and 128");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->io_threads = (uint32_t)count;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withConnectionsPerHost)
{
    zval *core = NULL;
    zval *max = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &core, &max) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(core) == IS_LONG && Z_LVAL_P(core) < 129 && Z_LVAL_P(core) > 0)
    {
        self->core_connections_per_host = Z_LVAL_P(core);
    }
    else
    {
        INVALID_ARGUMENT(core, "a number between 1 and 128");
    }

    if (max == NULL || Z_TYPE_P(max) == IS_NULL)
    {
        self->max_connections_per_host = Z_LVAL_P(core);
    }
    else if (Z_TYPE_P(max) == IS_LONG)
    {
        if (Z_LVAL_P(max) < Z_LVAL_P(core))
        {
            INVALID_ARGUMENT(max, "greater than core");
        }
        else if (Z_LVAL_P(max) > 128)
        {
            INVALID_ARGUMENT(max, "less than 128");
        }
        else
        {
            self->max_connections_per_host = Z_LVAL_P(max);
        }
    }
    else
    {
        INVALID_ARGUMENT(max, "a number between 1 and 128");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withReconnectInterval)
{
    zend_long interval = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(interval)
    ZEND_PARSE_PARAMETERS_END();

    if (interval < 1)
    {
        zval val;
        ZVAL_LONG(&val, interval);
        throw_invalid_argument(&val, "interval", "a positive number");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->reconnect_interval = interval * 1000;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withLatencyAwareRouting)
{
    zend_bool enabled = true;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(enabled)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->enable_latency_aware_routing = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withTCPNodelay)
{
    zend_bool enabled = true;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(enabled)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->enable_tcp_nodelay = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withTCPKeepalive)
{
    zend_long delay = 0;
    bool is_null = false;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG_OR_NULL(delay, is_null)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (is_null)
    {
        self->enable_tcp_keepalive = cass_false;
        self->tcp_keepalive_delay = 0;
        RETURN_ZVAL(getThis(), 1, 0);
    }

    if (delay < 1)
    {
        zval val;
        ZVAL_LONG(&val, delay);
        throw_invalid_argument(&val, "delay", "a positive number or null");
        return;
    }

    self->enable_tcp_keepalive = cass_true;
    self->tcp_keepalive_delay = delay * 1000;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withRetryPolicy)
{
    zval *retry_policy = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_OBJECT_OF_CLASS(retry_policy, php_driver_retry_policy_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    php_driver_retry_policy *policy = PHP_DRIVER_GET_RETRY_POLICY(retry_policy);

    if (self->retry_policy != NULL)
    {
        zend_object_release(&self->retry_policy->zval);
    }

    self->retry_policy = policy;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withTimestampGenerator)
{
    zval *timestamp_gen = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_OBJECT_OF_CLASS(timestamp_gen, php_driver_timestamp_gen_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (self->timestamp_gen != NULL)
    {
        zend_object_release(&self->timestamp_gen->zval);
    }

    php_driver_timestamp_gen *timestamp_generator = php_driver_timestamp_gen_object_fetch(Z_OBJ_P(timestamp_gen));
    self->timestamp_gen = timestamp_generator;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withSchemaMetadata)
{
    zend_bool enabled = true;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(enabled)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->enable_schema = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withHostnameResolution)
{
    zend_bool enabled = true;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(enabled)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->enable_hostname_resolution = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withRandomizedContactPoints)
{
    zend_bool enabled = true;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(enabled)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->enable_randomized_contact_points = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withConnectionHeartbeatInterval)
{
    zend_long interval = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(interval)
    ZEND_PARSE_PARAMETERS_END();

    if (interval < 0)
    {
        zval val;
        ZVAL_LONG(&val, interval);
        throw_invalid_argument(&val, "interval", "a positive number (or 0 to disable)");
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());
    self->connection_heartbeat_interval = interval;
    RETURN_ZVAL(getThis(), 1, 0);
}

void php_driver_define_ClusterBuilder()
{
    php_driver_cluster_builder_ce = register_class_Cassandra_Cluster_Builder();
    php_driver_initialize_cluster_builder_handlers();

    php_driver_cluster_builder_ce->create_object = php_driver_cluster_builder_new;
}
END_EXTERN_C()