/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 052102de30e25a8f509da7eb21d1a8aa3ba4fe37 */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_connect, 0, 0, Cassandra\\Session, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, keyspace, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Cassandra_Cluster_connectAsync, 0, 0, Cassandra\\Future, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, keyspace, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()




static const zend_function_entry class_Cassandra_Cluster_methods[] = {
	ZEND_ABSTRACT_ME_WITH_FLAGS(Cassandra_Cluster, connect, arginfo_class_Cassandra_Cluster_connect, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_ABSTRACT_ME_WITH_FLAGS(Cassandra_Cluster, connectAsync, arginfo_class_Cassandra_Cluster_connectAsync, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	ZEND_FE_END
};

static zend_class_entry *register_class_Cassandra_Cluster(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Cassandra", "Cluster", class_Cassandra_Cluster_methods);
	class_entry = zend_register_internal_interface(&ce);

	return class_entry;
}
