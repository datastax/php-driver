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
#include "BuilderHandlers.h"
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
            self->connection_heartbeat_interval, SAFE_ZEND_STRING(self->whitelist_hosts), SAFE_STR(self->whitelist_dcs),
            SAFE_ZEND_STRING(self->blacklist_hosts), SAFE_STR(self->blacklist_dcs));

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

    if (self->timestamp_gen != NULL)
    {
        php_driver_timestamp_gen *timestamp_gen = self->timestamp_gen;
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
            return;
        }

        if (i > 0)
        {
            smart_str_appendl(&contactPoints, ",", 1);
        }

        smart_str_appendl(&contactPoints, Z_STRVAL_P(host), Z_STRLEN_P(host));
    }

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

static zend_string *php_driver_build_hosts_str(zval *args, size_t argc)
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

ZEND_METHOD(Cassandra_Cluster_Builder, withBlackListHosts)
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

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (self->blacklist_hosts != zend_empty_string)
    {
        zend_string_release(self->blacklist_hosts);
    }

    self->blacklist_hosts = hosts;

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_METHOD(Cassandra_Cluster_Builder, withWhiteListHosts)
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

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (self->whitelist_hosts != NULL)
    {
        zend_string_release(self->whitelist_hosts);
    }

    self->blacklist_hosts = hosts;

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

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_OBJECT_OF_CLASS(timestamp_gen, php_driver_timestamp_gen_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_driver_cluster_builder *self = PHP_DRIVER_GET_CLUSTER_BUILDER(getThis());

    if (self->timestamp_gen != NULL)
    {
        zend_object_release(&self->timestamp_gen->zval);
    }

    php_driver_timestamp_gen *timestamp_generator = php_driver_timestamp_gen_object_fetch(Z_OBJ_P(timestamp_gen));
    GC_ADDREF(&timestamp_generator->zval);
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