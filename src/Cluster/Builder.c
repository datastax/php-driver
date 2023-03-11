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

#include <math.h>
#include <zend_smart_str.h>

#include <cassandra.h>

#include "php_driver.h"
#include "php_driver_globals.h"
#include "php_driver_types.h"
#include "util/consistency.h"

#include "Builder.h"
#include "Builder_arginfo.h"
#include "zend_portability.h"

BEGIN_EXTERN_C()
zend_class_entry *php_driver_cluster_builder_ce = NULL;

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
            self->contact_points, self->port, self->load_balancing_policy, SAFE_STR(self->local_dc),
            self->used_hosts_per_remote_dc, self->allow_remote_dcs_for_local_cl, self->use_token_aware_routing,
            SAFE_STR(self->username), SAFE_STR(self->password), self->connect_timeout, self->request_timeout,
            self->protocol_version, self->io_threads, self->core_connections_per_host, self->max_connections_per_host,
            self->reconnect_interval, self->enable_latency_aware_routing, self->enable_tcp_nodelay,
            self->enable_tcp_keepalive, self->tcp_keepalive_delay, self->enable_schema,
            self->enable_hostname_resolution, self->enable_randomized_contact_points,
            self->connection_heartbeat_interval, SAFE_STR(self->whitelist_hosts), SAFE_STR(self->whitelist_dcs),
            ZSTR_VAL(self->blacklist_hosts), SAFE_STR(self->blacklist_dcs));

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
        ASSERT_SUCCESS(cass_cluster_set_load_balance_dc_aware(
            cluster->cluster, self->local_dc, self->used_hosts_per_remote_dc, self->allow_remote_dcs_for_local_cl));
        break;
    }

    if (self->blacklist_hosts != zend_empty_string)
    {
        cass_cluster_set_blacklist_filtering(cluster->cluster, ZSTR_VAL(self->blacklist_hosts));
    }

    if (self->whitelist_hosts != NULL)
    {
        cass_cluster_set_whitelist_filtering(cluster->cluster, self->whitelist_hosts);
    }

    if (self->blacklist_dcs != NULL)
    {
        cass_cluster_set_blacklist_dc_filtering(cluster->cluster, self->blacklist_dcs);
    }

    if (self->whitelist_dcs != NULL)
    {
        cass_cluster_set_whitelist_dc_filtering(cluster->cluster, self->whitelist_dcs);
    }

    cass_cluster_set_token_aware_routing(cluster->cluster, self->use_token_aware_routing);

    if (self->username)
    {
        cass_cluster_set_credentials(cluster->cluster, self->username, self->password);
    }

    cass_cluster_set_connect_timeout(cluster->cluster, self->connect_timeout);
    cass_cluster_set_request_timeout(cluster->cluster, self->request_timeout);

    if (!Z_ISUNDEF(self->ssl_options))
    {
        php_driver_ssl *options = PHP_DRIVER_GET_SSL(&self->ssl_options);
        cass_cluster_set_ssl(cluster->cluster, options->ssl);
    }

    ASSERT_SUCCESS(cass_cluster_set_contact_points(cluster->cluster, self->contact_points));
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

    if (!Z_ISUNDEF(self->timestamp_gen))
    {
        php_driver_timestamp_gen *timestamp_gen = PHP_DRIVER_GET_TIMESTAMP_GEN(&self->timestamp_gen);
        cass_cluster_set_timestamp_gen(cluster->cluster, timestamp_gen->gen);
    }

    if (!Z_ISUNDEF(self->retry_policy))
    {
        php_driver_retry_policy *retry_policy = PHP_DRIVER_GET_RETRY_POLICY(PHP5TO7_ZVAL_MAYBE_P(self->retry_policy));
        cass_cluster_set_retry_policy(cluster->cluster, retry_policy->policy);
    }

    if (self->persist)
    {
        php5to7_zend_resource_le resource;

        ZVAL_NEW_PERSISTENT_RES(&resource, 0, cluster->cluster, php_le_php_driver_cluster());

        PHP5TO7_ZEND_HASH_UPDATE(&EG(persistent_list), cluster->hash_key, cluster->hash_key_len + 1, &resource,
                                 sizeof(php5to7_zend_resource_le));
        PHP_DRIVER_G(persistent_clusters)
        ++;
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
    zval *host = NULL;
    php5to7_zval_args args = NULL;
    int argc = 0, i;
    smart_str contactPoints = PHP5TO7_SMART_STR_INIT;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE)
    {
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    for (i = 0; i < argc; i++)
    {
        host = PHP5TO7_ZVAL_ARG(args[i]);

        if (Z_TYPE_P(host) != IS_STRING)
        {
            smart_str_free(&contactPoints);
            throw_invalid_argument(host, "host", "a string ip address or hostname");
            PHP5TO7_MAYBE_EFREE(args);
            return;
        }

        if (i > 0)
        {
            smart_str_appendl(&contactPoints, ",", 1);
        }

        smart_str_appendl(&contactPoints, Z_STRVAL_P(host), Z_STRLEN_P(host));
    }

    PHP5TO7_MAYBE_EFREE(args);
    smart_str_0(&contactPoints);

    efree(self->contact_points);
    self->contact_points = estrndup(contactPoints.s->val, contactPoints.s->len);
    smart_str_free(&contactPoints);

    RETURN_ZVAL(getThis(), 1, 0);
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

    if (self->local_dc != NULL)
    {
        efree(self->local_dc);
        self->local_dc = NULL;
    }

    self->load_balancing_policy = LOAD_BALANCING_ROUND_ROBIN;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withDatacenterAwareRoundRobinLoadBalancingPolicy)
{
    char *local_dc;
    size_t local_dc_len;
    zval *hostPerRemoteDatacenter = NULL;
    zend_bool allow_remote_dcs_for_local_cl;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "szb", &local_dc, &local_dc_len, &hostPerRemoteDatacenter,
                              &allow_remote_dcs_for_local_cl) == FAILURE)
    {
        return;
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(hostPerRemoteDatacenter) != IS_LONG || Z_LVAL_P(hostPerRemoteDatacenter) < 0)
    {
        INVALID_ARGUMENT(hostPerRemoteDatacenter, "a positive integer");
    }

    if (self->local_dc)
    {
        efree(self->local_dc);
        self->local_dc = NULL;
    }

    self->load_balancing_policy = LOAD_BALANCING_DC_AWARE_ROUND_ROBIN;
    self->local_dc = estrndup(local_dc, local_dc_len);
    self->used_hosts_per_remote_dc = Z_LVAL_P(hostPerRemoteDatacenter);
    self->allow_remote_dcs_for_local_cl = allow_remote_dcs_for_local_cl;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withBlackListHosts)
{
    zval *args = NULL;
    int argc = 0;

    ZEND_PARSE_PARAMETERS_START(1, -1)
    Z_PARAM_VARIADIC('+', args, argc)
    ZEND_PARSE_PARAMETERS_END();

    smart_str blacklist_hosts = {0};

    for (size_t i = 0; i < argc; i++)
    {
        zval *host = &args[i];

        if (Z_TYPE_P(host) != IS_STRING)
        {
            smart_str_free(&blacklist_hosts);
            throw_invalid_argument(host, "hosts", "a string ip address or hostname");
            PHP5TO7_MAYBE_EFREE(args);
            return;
        }

        if (i > 0)
        {
            smart_str_appendl(&blacklist_hosts, ",", 1);
        }

        smart_str_append(&blacklist_hosts, Z_STR_P(host));
    }

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (self->blacklist_hosts != zend_empty_string)
    {
        zend_string_release(self->blacklist_hosts);
    }

    self->blacklist_hosts = smart_str_extract(&blacklist_hosts);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withWhiteListHosts)
{
    zval *hosts = NULL;
    php5to7_zval_args args = NULL;
    int argc = 0, i;
    smart_str whitelist_hosts = PHP5TO7_SMART_STR_INIT;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    for (i = 0; i < argc; i++)
    {
        hosts = PHP5TO7_ZVAL_ARG(args[i]);

        if (Z_TYPE_P(hosts) != IS_STRING)
        {
            smart_str_free(&whitelist_hosts);
            throw_invalid_argument(hosts, "hosts", "a string ip address or hostname");
            PHP5TO7_MAYBE_EFREE(args);
            return;
        }

        if (i > 0)
        {
            smart_str_appendl(&whitelist_hosts, ",", 1);
        }

        smart_str_appendl(&whitelist_hosts, Z_STRVAL_P(hosts), Z_STRLEN_P(hosts));
    }

    PHP5TO7_MAYBE_EFREE(args);
    smart_str_0(&whitelist_hosts);

    efree(self->whitelist_hosts);
    self->whitelist_hosts = estrndup(whitelist_hosts.s->val, whitelist_hosts.s->len);
    smart_str_free(&whitelist_hosts);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withBlackListDCs)
{
    zval *dcs = NULL;
    php5to7_zval_args args = NULL;
    int argc = 0, i;
    smart_str blacklist_dcs = PHP5TO7_SMART_STR_INIT;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    for (i = 0; i < argc; i++)
    {
        dcs = PHP5TO7_ZVAL_ARG(args[i]);

        if (Z_TYPE_P(dcs) != IS_STRING)
        {
            smart_str_free(&blacklist_dcs);
            throw_invalid_argument(dcs, "dcs", "a string");
            PHP5TO7_MAYBE_EFREE(args);
            return;
        }

        if (i > 0)
        {
            smart_str_appendl(&blacklist_dcs, ",", 1);
        }

        smart_str_appendl(&blacklist_dcs, Z_STRVAL_P(dcs), Z_STRLEN_P(dcs));
    }

    PHP5TO7_MAYBE_EFREE(args);
    smart_str_0(&blacklist_dcs);

    efree(self->blacklist_dcs);
    self->blacklist_dcs = estrndup(blacklist_dcs.s->val, blacklist_dcs.s->len);
    smart_str_free(&blacklist_dcs);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withWhiteListDCs)
{
    zval *dcs = NULL;
    php5to7_zval_args args = NULL;
    int argc = 0, i;
    smart_str whitelist_dcs = PHP5TO7_SMART_STR_INIT;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    for (i = 0; i < argc; i++)
    {
        dcs = PHP5TO7_ZVAL_ARG(args[i]);

        if (Z_TYPE_P(dcs) != IS_STRING)
        {
            smart_str_free(&whitelist_dcs);
            throw_invalid_argument(dcs, "dcs", "a string");
            PHP5TO7_MAYBE_EFREE(args);
            return;
        }

        if (i > 0)
        {
            smart_str_appendl(&whitelist_dcs, ",", 1);
        }

        smart_str_appendl(&whitelist_dcs, Z_STRVAL_P(dcs), Z_STRLEN_P(dcs));
    }

    PHP5TO7_MAYBE_EFREE(args);
    smart_str_0(&whitelist_dcs);

    efree(self->whitelist_dcs);
    self->whitelist_dcs = estrndup(whitelist_dcs.s->val, whitelist_dcs.s->len);
    smart_str_free(&whitelist_dcs);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withTokenAwareRouting)
{
    zend_bool enabled = 1;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enabled) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->use_token_aware_routing = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withCredentials)
{
    zval *username = NULL;
    zval *password = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &username, &password) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(username) != IS_STRING)
    {
        INVALID_ARGUMENT(username, "a string");
    }

    if (Z_TYPE_P(password) != IS_STRING)
    {
        INVALID_ARGUMENT(password, "a string");
    }

    if (self->username)
    {
        efree(self->username);
        efree(self->password);
    }

    self->username = estrndup(Z_STRVAL_P(username), Z_STRLEN_P(username));
    self->password = estrndup(Z_STRVAL_P(password), Z_STRLEN_P(password));

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withConnectTimeout)
{
    zval *timeout = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &timeout) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(timeout) == IS_LONG && Z_LVAL_P(timeout) > 0)
    {
        self->connect_timeout = Z_LVAL_P(timeout) * 1000;
    }
    else if (Z_TYPE_P(timeout) == IS_DOUBLE && Z_DVAL_P(timeout) > 0)
    {
        self->connect_timeout = ceil(Z_DVAL_P(timeout) * 1000);
    }
    else
    {
        INVALID_ARGUMENT(timeout, "a positive number");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withRequestTimeout)
{
    double timeout;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "d", &timeout) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->request_timeout = ceil(timeout * 1000);

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
    zend_bool enabled = 1;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enabled) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->persist = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withProtocolVersion)
{
    zval *version = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &version) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(version) == IS_LONG && Z_LVAL_P(version) >= 1)
    {
        self->protocol_version = Z_LVAL_P(version);
    }
    else
    {
        INVALID_ARGUMENT(version, "must be >= 1");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withIOThreads)
{
    zval *count = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &count) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(count) == IS_LONG && Z_LVAL_P(count) < 129 && Z_LVAL_P(count) > 0)
    {
        self->io_threads = Z_LVAL_P(count);
    }
    else
    {
        INVALID_ARGUMENT(count, "a number between 1 and 128");
    }

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
    zval *interval = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &interval) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(interval) == IS_LONG && Z_LVAL_P(interval) > 0)
    {
        self->reconnect_interval = Z_LVAL_P(interval) * 1000;
    }
    else if (Z_TYPE_P(interval) == IS_DOUBLE && Z_DVAL_P(interval) > 0)
    {
        self->reconnect_interval = ceil(Z_DVAL_P(interval) * 1000);
    }
    else
    {
        INVALID_ARGUMENT(interval, "a positive number");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withLatencyAwareRouting)
{
    zend_bool enabled = 1;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enabled) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->enable_latency_aware_routing = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withTCPNodelay)
{
    zend_bool enabled = 1;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enabled) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->enable_tcp_nodelay = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withTCPKeepalive)
{
    zval *delay = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &delay) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(delay) == IS_NULL)
    {
        self->enable_tcp_keepalive = 0;
        self->tcp_keepalive_delay = 0;
    }
    else if (Z_TYPE_P(delay) == IS_LONG && Z_LVAL_P(delay) > 0)
    {
        self->enable_tcp_keepalive = 1;
        self->tcp_keepalive_delay = Z_LVAL_P(delay) * 1000;
    }
    else if (Z_TYPE_P(delay) == IS_DOUBLE && Z_DVAL_P(delay) > 0)
    {
        self->enable_tcp_keepalive = 1;
        self->tcp_keepalive_delay = ceil(Z_DVAL_P(delay) * 1000);
    }
    else
    {
        INVALID_ARGUMENT(delay, "a positive number or null");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withRetryPolicy)
{
    zval *retry_policy = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &retry_policy, php_driver_retry_policy_ce) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (!Z_ISUNDEF(self->retry_policy))
        zval_ptr_dtor(&self->retry_policy);

    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(self->retry_policy), retry_policy);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withTimestampGenerator)
{
    zval *timestamp_gen = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &timestamp_gen, php_driver_timestamp_gen_ce) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (!Z_ISUNDEF(self->timestamp_gen))
        zval_ptr_dtor(&self->timestamp_gen);

    PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(self->timestamp_gen), timestamp_gen);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withSchemaMetadata)
{
    zend_bool enabled = 1;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enabled) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->enable_schema = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withHostnameResolution)
{
    zend_bool enabled = 1;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enabled) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->enable_hostname_resolution = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withRandomizedContactPoints)
{
    zend_bool enabled = 1;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &enabled) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    self->enable_randomized_contact_points = enabled;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withConnectionHeartbeatInterval)
{
    zval *interval = NULL;
    php_driver_cluster_builder *self;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &interval) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (Z_TYPE_P(interval) == IS_LONG && Z_LVAL_P(interval) >= 0)
    {
        self->connection_heartbeat_interval = Z_LVAL_P(interval);
    }
    else if (Z_TYPE_P(interval) == IS_DOUBLE && Z_DVAL_P(interval) >= 0)
    {
        self->connection_heartbeat_interval = ceil(Z_DVAL_P(interval));
    }
    else
    {
        INVALID_ARGUMENT(interval, "a positive number (or 0 to disable)");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

static zend_object_handlers php_driver_cluster_builder_handlers;

static HashTable *php_driver_cluster_builder_gc(zend_object *object, php5to7_zval_gc table, int *n)
{
    *table = NULL;
    *n = 0;
    return zend_std_get_properties(object);
}

static HashTable *php_driver_cluster_builder_properties(zend_object *object)
{
    php5to7_zval contactPoints;
    php5to7_zval loadBalancingPolicy;
    php5to7_zval localDatacenter;
    php5to7_zval hostPerRemoteDatacenter;
    php5to7_zval useRemoteDatacenterForLocalConsistencies;
    php5to7_zval useTokenAwareRouting;
    php5to7_zval username;
    php5to7_zval password;
    php5to7_zval connectTimeout;
    php5to7_zval requestTimeout;
    php5to7_zval sslOptions;
    php5to7_zval defaultConsistency;
    php5to7_zval defaultPageSize;
    php5to7_zval defaultTimeout;
    php5to7_zval usePersistentSessions;
    php5to7_zval protocolVersion;
    php5to7_zval ioThreads;
    php5to7_zval coreConnectionPerHost;
    php5to7_zval maxConnectionsPerHost;
    php5to7_zval reconnectInterval;
    php5to7_zval latencyAwareRouting;
    php5to7_zval tcpNodelay;
    php5to7_zval tcpKeepalive;
    php5to7_zval retryPolicy;
    php5to7_zval blacklistHosts;
    php5to7_zval whitelistHosts;
    php5to7_zval blacklistDCs;
    php5to7_zval whitelistDCs;
    php5to7_zval timestampGen;
    php5to7_zval schemaMetadata;
    php5to7_zval hostnameResolution;
    php5to7_zval randomizedContactPoints;
    php5to7_zval connectionHeartbeatInterval;

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

    if (self->blacklist_hosts != zend_empty_string)
    {
        ZVAL_STR(&blacklistHosts, self->blacklist_hosts);
    }
    else
    {
        ZVAL_NULL(&blacklistHosts);
    }

    if (self->whitelist_hosts)
    {
        ZVAL_STRING(&whitelistHosts, self->whitelist_hosts);
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

    if (!Z_ISUNDEF(self->timestamp_gen))
    {
        PHP5TO7_ZVAL_COPY(&timestampGen, &self->timestamp_gen);
    }
    else
    {
        ZVAL_NULL(&timestampGen);
    }

    PHP5TO7_ZVAL_MAYBE_MAKE(schemaMetadata);
    ZVAL_BOOL(&schemaMetadata, self->enable_schema);

    PHP5TO7_ZVAL_MAYBE_MAKE(hostnameResolution);
    ZVAL_BOOL(&hostnameResolution, self->enable_hostname_resolution);

    PHP5TO7_ZVAL_MAYBE_MAKE(randomizedContactPoints);
    ZVAL_BOOL(&randomizedContactPoints, self->enable_randomized_contact_points);

    PHP5TO7_ZVAL_MAYBE_MAKE(connectionHeartbeatInterval);
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

static void php_driver_cluster_builder_free(php5to7_zend_object_free *object)
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

    if (self->whitelist_hosts)
    {
        efree(self->whitelist_hosts);
        self->whitelist_hosts = NULL;
    }

    if (self->blacklist_hosts != zend_empty_string)
    {
        zend_string_release(self->blacklist_hosts);
        self->blacklist_hosts = zend_empty_string;
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

    PHP5TO7_ZVAL_MAYBE_DESTROY(self->ssl_options);
    PHP5TO7_ZVAL_MAYBE_DESTROY(self->default_timeout);
    PHP5TO7_ZVAL_MAYBE_DESTROY(self->retry_policy);
    PHP5TO7_ZVAL_MAYBE_DESTROY(self->timestamp_gen);

    zend_objects_destroy_object(&self->zval);
    efree(self);
}

static php5to7_zend_object php_driver_cluster_builder_new(zend_class_entry *ce)
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
    self->blacklist_hosts = zend_empty_string;
    self->whitelist_hosts = NULL;
    self->blacklist_dcs = NULL;
    self->whitelist_dcs = NULL;
    self->enable_hostname_resolution = 0;
    self->enable_randomized_contact_points = 1;
    self->connection_heartbeat_interval = 30;

    ZVAL_UNDEF(&self->ssl_options);
    ZVAL_UNDEF(&self->default_timeout);
    ZVAL_UNDEF(&self->retry_policy);
    ZVAL_UNDEF(&self->timestamp_gen);

    zend_object_std_init(&self->zval, ce);
    self->zval.handlers = &php_driver_cluster_builder_handlers;

    if (zend_object_properties_size(ce) > 0)
    {
        object_properties_init(&self->zval, ce);
    }

    return &self->zval;
}
void php_driver_define_ClusterBuilder()
{
    memcpy(&php_driver_cluster_builder_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    php_driver_cluster_builder_ce = register_class_Cassandra_Cluster_Builder();
    php_driver_cluster_builder_ce->create_object = php_driver_cluster_builder_new;
    php_driver_cluster_builder_handlers.get_properties = php_driver_cluster_builder_properties;
    php_driver_cluster_builder_handlers.get_gc = php_driver_cluster_builder_gc;
    php_driver_cluster_builder_handlers.compare = php_driver_cluster_builder_compare;
    php_driver_cluster_builder_handlers.offset = XtOffsetOf(php_driver_cluster_builder, zval);
    php_driver_cluster_builder_handlers.free_obj = php_driver_cluster_builder_free;
}
END_EXTERN_C()