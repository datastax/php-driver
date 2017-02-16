#!/bin/sh
php -d extension=cassandra.so -d extension_dir=../modules/ generate_doc_yaml.php ..
