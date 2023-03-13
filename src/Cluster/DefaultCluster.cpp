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

#include <php_driver.h>
#include <php_driver_globals.h>
#include <php_driver_types.h>
#include <util/future.h>
#include <util/ref.h>

#include "Cluster.h"
#include "DefaultClusterHandlers.h"

BEGIN_EXTERN_C()

#include "DefaultCluster_arginfo.h"

zend_class_entry *php_driver_default_cluster_ce = nullptr;

static void free_session(void *session)
{
    cass_session_free((CassSession *)session);
}

ZEND_METHOD(Cassandra_DefaultCluster, connect)
{
    char *keyspace = nullptr;
    php5to7_size keyspace_len;
    zval *timeout = nullptr;
    php_driver_cluster *self;
    php_driver_session *session;
    CassFuture *future = nullptr;
    char *hash_key;
    php5to7_size hash_key_len = 0;
    php_driver_psession *psession;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sz", &keyspace, &keyspace_len, &timeout) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER(getThis());

    object_init_ex(return_value, php_driver_default_session_ce);
    session = PHP_DRIVER_GET_SESSION(return_value);

    session->default_consistency = self->default_consistency;
    session->default_page_size = self->default_page_size;
    session->persist = self->persist;
    session->hash_key = self->hash_key;
    session->keyspace = keyspace;

    if (!PHP5TO7_ZVAL_IS_UNDEF(session->default_timeout))
    {
        PHP5TO7_ZVAL_COPY(PHP5TO7_ZVAL_MAYBE_P(session->default_timeout), PHP5TO7_ZVAL_MAYBE_P(self->default_timeout));
    }

    if (session->persist)
    {
        php5to7_zend_resource_le *le;

        hash_key_len = spprintf(&hash_key, 0, "%s:session:%s", self->hash_key, SAFE_STR(keyspace));

        if (PHP5TO7_ZEND_HASH_FIND(&EG(persistent_list), hash_key, hash_key_len + 1, le) &&
            Z_RES_P(le)->type == php_le_php_driver_session())
        {
            psession = (php_driver_psession *)Z_RES_P(le)->ptr;
            session->session = php_driver_add_ref(psession->session);
            future = psession->future;
        }
    }

    if (future == NULL)
    {
        php5to7_zend_resource_le resource;

        session->session = php_driver_new_peref(cass_session_new(), free_session, 1);

        if (keyspace)
        {
            future = cass_session_connect_keyspace((CassSession *)session->session->data, self->cluster, keyspace);
        }
        else
        {
            future = cass_session_connect((CassSession *)session->session->data, self->cluster);
        }

        if (session->persist)
        {
            psession = (php_driver_psession *)pecalloc(1, sizeof(php_driver_psession), 1);
            psession->session = php_driver_add_ref(session->session);
            psession->future = future;

            ZVAL_NEW_PERSISTENT_RES(&resource, 0, psession, php_le_php_driver_session());
            PHP5TO7_ZEND_HASH_UPDATE(&EG(persistent_list), hash_key, hash_key_len + 1, &resource,
                                     sizeof(php5to7_zend_resource_le));
            PHP_DRIVER_G(persistent_sessions)++;
        }
    }

    if (php_driver_future_wait_timed(future, timeout) == FAILURE)
    {
        if (session->persist)
        {
            efree(hash_key);
        }
        else
        {
            cass_future_free(future);
        }

        return;
    }

    if (php_driver_future_is_error(future) == FAILURE)
    {
        if (session->persist)
        {
            (void)PHP5TO7_ZEND_HASH_DEL(&EG(persistent_list), hash_key, hash_key_len + 1);
            efree(hash_key);
        }
        else
        {
            cass_future_free(future);
        }

        return;
    }

    if (session->persist)
        efree(hash_key);
}

ZEND_METHOD(Cassandra_DefaultCluster, connectAsync)
{
    char *hash_key = NULL;
    php5to7_size hash_key_len = 0;
    char *keyspace = NULL;
    php5to7_size keyspace_len;
    php_driver_cluster *self = NULL;
    php_driver_future_session *future = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &keyspace, &keyspace_len) == FAILURE)
    {
        return;
    }

    self = PHP_DRIVER_GET_CLUSTER(getThis());

    object_init_ex(return_value, php_driver_future_session_ce);
    future = PHP_DRIVER_GET_FUTURE_SESSION(return_value);

    future->persist = self->persist;

    if (self->persist)
    {
        php5to7_zend_resource_le *le;

        hash_key_len = spprintf(&hash_key, 0, "%s:session:%s", self->hash_key, SAFE_STR(keyspace));

        future->session_hash_key = self->hash_key;
        future->session_keyspace = keyspace;
        future->hash_key = hash_key;
        future->hash_key_len = hash_key_len;

        if (PHP5TO7_ZEND_HASH_FIND(&EG(persistent_list), hash_key, hash_key_len + 1, le) &&
            Z_RES_P(le)->type == php_le_php_driver_session())
        {
            php_driver_psession *psession = (php_driver_psession *)Z_RES_P(le)->ptr;
            future->session = php_driver_add_ref(psession->session);
            future->future = psession->future;
            return;
        }
    }

    future->session = php_driver_new_peref(cass_session_new(), free_session, 1);

    if (keyspace)
    {
        future->future = cass_session_connect_keyspace((CassSession *)future->session->data, self->cluster, keyspace);
    }
    else
    {
        future->future = cass_session_connect((CassSession *)future->session->data, self->cluster);
    }

    if (self->persist)
    {
        php5to7_zend_resource_le resource;
        auto *psession = (php_driver_psession *)pecalloc(1, sizeof(php_driver_psession), 1);
        psession->session = php_driver_add_ref(future->session);
        psession->future = future->future;

        ZVAL_NEW_PERSISTENT_RES(&resource, 0, psession, php_le_php_driver_session());
        PHP5TO7_ZEND_HASH_UPDATE(&EG(persistent_list), hash_key, hash_key_len + 1, &resource,
                                 sizeof(php5to7_zend_resource_le));
        PHP_DRIVER_G(persistent_sessions)++;
    }
}

END_EXTERN_C()

void php_driver_define_DefaultCluster()
{
    php_driver_default_cluster_ce = register_class_Cassandra_DefaultCluster(php_driver_cluster_ce);
    php_driver_initialize_default_cluster_handlers();

    php_driver_default_cluster_ce->create_object = php_driver_default_cluster_new;
}