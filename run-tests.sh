#!/usr/bin/env bash
# -*- coding: utf-8 -*-

export SCYLLADB_HOSTS="scylladb"

# Just not to override the existing files
cp -r "$PWD" /tmp/scylladb-php-driver || exit 1
cd /tmp/scylladb-php-driver || exit 1

rm -rf out/ || exit 1
rm -rf vendor || exit 1
cmake --preset Release

pushd out/Release || exit 1
ninja install || exit 1

popd || exit 1
cp cassandra.ini /usr/local/etc/php/conf.d/cassandra.ini
composer install
php ./vendor/bin/pest
