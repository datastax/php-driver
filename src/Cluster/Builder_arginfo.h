/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a97ccbde66cce65296c03272d8609ee5d1ba361a */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withDefaultConsistency, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, consistency, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withDefaultPageSize, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, pageSize, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withDefaultTimeout, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withContactPoints, 0, 0, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, host, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withPort, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withRoundRobinLoadBalancingPolicy, 0, 0, Cassandra\\Cluster\\Builder, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withDatacenterAwareRoundRobinLoadBalancingPolicy, 0, 3, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, localDatacenter, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, hostPerRemoteDatacenter, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, useRemoteDatacenterForLocalConsistencies, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withBlackListHosts, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, hosts, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Cassandra_Cluster_Builder_withWhiteListHosts arginfo_class_Cassandra_Cluster_Builder_withBlackListHosts

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withBlackListDCs, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, dcs, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Cassandra_Cluster_Builder_withWhiteListDCs arginfo_class_Cassandra_Cluster_Builder_withBlackListDCs

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withTokenAwareRouting, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, enabled, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withCredentials, 0, 2, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Cassandra_Cluster_Builder_withConnectTimeout arginfo_class_Cassandra_Cluster_Builder_withDefaultTimeout

#define arginfo_class_Cassandra_Cluster_Builder_withRequestTimeout arginfo_class_Cassandra_Cluster_Builder_withDefaultTimeout

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withSSL, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_OBJ_INFO(0, options, Cassandra\\SSLOptions, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Cassandra_Cluster_Builder_withPersistentSessions arginfo_class_Cassandra_Cluster_Builder_withTokenAwareRouting

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withProtocolVersion, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withIOThreads, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withConnectionsPerHost, 0, 2, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, core, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withReconnectInterval, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, interval, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Cassandra_Cluster_Builder_withLatencyAwareRouting arginfo_class_Cassandra_Cluster_Builder_withTokenAwareRouting

#define arginfo_class_Cassandra_Cluster_Builder_withTCPNodelay arginfo_class_Cassandra_Cluster_Builder_withTokenAwareRouting

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withTCPKeepalive, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, delay, IS_DOUBLE, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withRetryPolicy, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_OBJ_INFO(0, policy, Cassandra\\RetryPolicy, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withTimestampGenerator, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_OBJ_INFO(0, generator, Cassandra\\TimestampGenerator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withSchemaMetadata, 0, 0, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enabled, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_Cassandra_Cluster_Builder_withHostnameResolution arginfo_class_Cassandra_Cluster_Builder_withSchemaMetadata

#define arginfo_class_Cassandra_Cluster_Builder_withRandomizedContactPoints arginfo_class_Cassandra_Cluster_Builder_withSchemaMetadata

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_withConnectionHeartbeatInterval, 0, 1, Cassandra\\Cluster\\Builder, 0)
	ZEND_ARG_TYPE_INFO(0, interval, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_Builder_build, 0, 0, Cassandra\\Cluster, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Cassandra_Cluster_Builder, withDefaultConsistency);
ZEND_METHOD(Cassandra_Cluster_Builder, withDefaultPageSize);
ZEND_METHOD(Cassandra_Cluster_Builder, withDefaultTimeout);
ZEND_METHOD(Cassandra_Cluster_Builder, withContactPoints);
ZEND_METHOD(Cassandra_Cluster_Builder, withPort);
ZEND_METHOD(Cassandra_Cluster_Builder, withRoundRobinLoadBalancingPolicy);
ZEND_METHOD(Cassandra_Cluster_Builder, withDatacenterAwareRoundRobinLoadBalancingPolicy);
ZEND_METHOD(Cassandra_Cluster_Builder, withBlackListHosts);
ZEND_METHOD(Cassandra_Cluster_Builder, withWhiteListHosts);
ZEND_METHOD(Cassandra_Cluster_Builder, withBlackListDCs);
ZEND_METHOD(Cassandra_Cluster_Builder, withWhiteListDCs);
ZEND_METHOD(Cassandra_Cluster_Builder, withTokenAwareRouting);
ZEND_METHOD(Cassandra_Cluster_Builder, withCredentials);
ZEND_METHOD(Cassandra_Cluster_Builder, withConnectTimeout);
ZEND_METHOD(Cassandra_Cluster_Builder, withRequestTimeout);
ZEND_METHOD(Cassandra_Cluster_Builder, withSSL);
ZEND_METHOD(Cassandra_Cluster_Builder, withPersistentSessions);
ZEND_METHOD(Cassandra_Cluster_Builder, withProtocolVersion);
ZEND_METHOD(Cassandra_Cluster_Builder, withIOThreads);
ZEND_METHOD(Cassandra_Cluster_Builder, withConnectionsPerHost);
ZEND_METHOD(Cassandra_Cluster_Builder, withReconnectInterval);
ZEND_METHOD(Cassandra_Cluster_Builder, withLatencyAwareRouting);
ZEND_METHOD(Cassandra_Cluster_Builder, withTCPNodelay);
ZEND_METHOD(Cassandra_Cluster_Builder, withTCPKeepalive);
ZEND_METHOD(Cassandra_Cluster_Builder, withRetryPolicy);
ZEND_METHOD(Cassandra_Cluster_Builder, withTimestampGenerator);
ZEND_METHOD(Cassandra_Cluster_Builder, withSchemaMetadata);
ZEND_METHOD(Cassandra_Cluster_Builder, withHostnameResolution);
ZEND_METHOD(Cassandra_Cluster_Builder, withRandomizedContactPoints);
ZEND_METHOD(Cassandra_Cluster_Builder, withConnectionHeartbeatInterval);
ZEND_METHOD(Cassandra_Cluster_Builder, build);


static const zend_function_entry class_Cassandra_Cluster_Builder_methods[] = {
	ZEND_ME(Cassandra_Cluster_Builder, withDefaultConsistency, arginfo_class_Cassandra_Cluster_Builder_withDefaultConsistency, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withDefaultPageSize, arginfo_class_Cassandra_Cluster_Builder_withDefaultPageSize, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withDefaultTimeout, arginfo_class_Cassandra_Cluster_Builder_withDefaultTimeout, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withContactPoints, arginfo_class_Cassandra_Cluster_Builder_withContactPoints, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withPort, arginfo_class_Cassandra_Cluster_Builder_withPort, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withRoundRobinLoadBalancingPolicy, arginfo_class_Cassandra_Cluster_Builder_withRoundRobinLoadBalancingPolicy, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withDatacenterAwareRoundRobinLoadBalancingPolicy, arginfo_class_Cassandra_Cluster_Builder_withDatacenterAwareRoundRobinLoadBalancingPolicy, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withBlackListHosts, arginfo_class_Cassandra_Cluster_Builder_withBlackListHosts, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withWhiteListHosts, arginfo_class_Cassandra_Cluster_Builder_withWhiteListHosts, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withBlackListDCs, arginfo_class_Cassandra_Cluster_Builder_withBlackListDCs, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withWhiteListDCs, arginfo_class_Cassandra_Cluster_Builder_withWhiteListDCs, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withTokenAwareRouting, arginfo_class_Cassandra_Cluster_Builder_withTokenAwareRouting, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withCredentials, arginfo_class_Cassandra_Cluster_Builder_withCredentials, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withConnectTimeout, arginfo_class_Cassandra_Cluster_Builder_withConnectTimeout, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withRequestTimeout, arginfo_class_Cassandra_Cluster_Builder_withRequestTimeout, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withSSL, arginfo_class_Cassandra_Cluster_Builder_withSSL, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withPersistentSessions, arginfo_class_Cassandra_Cluster_Builder_withPersistentSessions, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withProtocolVersion, arginfo_class_Cassandra_Cluster_Builder_withProtocolVersion, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withIOThreads, arginfo_class_Cassandra_Cluster_Builder_withIOThreads, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withConnectionsPerHost, arginfo_class_Cassandra_Cluster_Builder_withConnectionsPerHost, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withReconnectInterval, arginfo_class_Cassandra_Cluster_Builder_withReconnectInterval, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withLatencyAwareRouting, arginfo_class_Cassandra_Cluster_Builder_withLatencyAwareRouting, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withTCPNodelay, arginfo_class_Cassandra_Cluster_Builder_withTCPNodelay, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withTCPKeepalive, arginfo_class_Cassandra_Cluster_Builder_withTCPKeepalive, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withRetryPolicy, arginfo_class_Cassandra_Cluster_Builder_withRetryPolicy, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withTimestampGenerator, arginfo_class_Cassandra_Cluster_Builder_withTimestampGenerator, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withSchemaMetadata, arginfo_class_Cassandra_Cluster_Builder_withSchemaMetadata, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withHostnameResolution, arginfo_class_Cassandra_Cluster_Builder_withHostnameResolution, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withRandomizedContactPoints, arginfo_class_Cassandra_Cluster_Builder_withRandomizedContactPoints, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, withConnectionHeartbeatInterval, arginfo_class_Cassandra_Cluster_Builder_withConnectionHeartbeatInterval, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_Cluster_Builder, build, arginfo_class_Cassandra_Cluster_Builder_build, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Cassandra_Cluster_Builder(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Cassandra\\Cluster", "Builder", class_Cassandra_Cluster_Builder_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	return class_entry;
}
