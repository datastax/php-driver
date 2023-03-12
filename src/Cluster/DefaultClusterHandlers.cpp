#include <php.h>
#include <php_driver.h>
#include <php_driver_types.h>

#include "DefaultClusterHandlers.h"

BEGIN_EXTERN_C()

static zend_object_handlers php_driver_default_cluster_handlers;

static HashTable *php_driver_default_cluster_properties(zend_object *object)
{
    HashTable *props = zend_std_get_properties(object);

    return props;
}

static int php_driver_default_cluster_compare(zval *obj1, zval *obj2)
{
    ZEND_COMPARE_OBJECTS_FALLBACK(obj1, obj2);

    if (Z_OBJCE_P(obj1) != Z_OBJCE_P(obj2))
        return 1; /* different classes */

    return Z_OBJ_HANDLE_P(obj1) != Z_OBJ_HANDLE_P(obj1);
}

static void php_driver_default_cluster_free(php5to7_zend_object_free *object)
{
    php_driver_cluster *self = PHP5TO7_ZEND_OBJECT_GET(cluster, object);

    if (self->persist)
    {
        efree(self->hash_key);
    }
    else if (self->cluster)
    {
        cass_cluster_free(self->cluster);
    }

    if (!Z_ISUNDEF(self->default_timeout))
    {
        zval_ptr_dtor(&self->default_timeout);
        ZVAL_UNDEF(&self->default_timeout);
    }
}

zend_object *php_driver_default_cluster_new(zend_class_entry *ce)
{
    auto *self = PHP5TO7_ZEND_OBJECT_ECALLOC(cluster, ce);

    self->cluster = nullptr;
    self->default_consistency = PHP_DRIVER_DEFAULT_CONSISTENCY;
    self->default_page_size = 5000;
    self->persist = cass_false;
    self->hash_key = nullptr;

    PHP5TO7_ZVAL_UNDEF(self->default_timeout);

    zend_object_std_init(&self->zval, ce);

    PHP5TO7_ZEND_OBJECT_INIT_EX(cluster, default_cluster, self, ce);
}

END_EXTERN_C()

void php_driver_initialize_default_cluster_handlers()
{
    memcpy(&php_driver_default_cluster_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    php_driver_default_cluster_handlers.get_properties = php_driver_default_cluster_properties;
    php_driver_default_cluster_handlers.compare = php_driver_default_cluster_compare;
    php_driver_default_cluster_handlers.free_obj = php_driver_default_cluster_free;
    php_driver_default_cluster_handlers.offset = XtOffsetOf(php_driver_cluster, zval);
}