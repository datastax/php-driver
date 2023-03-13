/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 28b2bb87b0daa0292013b034cd99925a5a9704aa */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_DefaultCluster_connect, 0, 0, Cassandra\\Session, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, keyspace, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_DefaultCluster_connectAsync, 0, 0, Cassandra\\Future, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, keyspace, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()


ZEND_METHOD(Cassandra_DefaultCluster, connect);
ZEND_METHOD(Cassandra_DefaultCluster, connectAsync);


static const zend_function_entry class_Cassandra_DefaultCluster_methods[] = {
	ZEND_ME(Cassandra_DefaultCluster, connect, arginfo_class_Cassandra_DefaultCluster_connect, ZEND_ACC_PUBLIC)
	ZEND_ME(Cassandra_DefaultCluster, connectAsync, arginfo_class_Cassandra_DefaultCluster_connectAsync, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Cassandra_DefaultCluster(zend_class_entry *class_entry_Cassandra_Cluster)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Cassandra", "DefaultCluster", class_Cassandra_DefaultCluster_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_class_implements(class_entry, 1, class_entry_Cassandra_Cluster);

	return class_entry;
}
