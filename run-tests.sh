#!/usr/bin/env bash
# -*- coding: utf-8 -*-

export SCYLLADB_HOSTS="scylladb"

rm -rf out/ || exit 1
rm -rf vendor || exit 1
cmake --preset Release

pushd out/Release || exit 1
ninja install

popd || exit 1
cp cassandra.ini /usr/local/etc/php/conf.d/cassandra.ini
composer install
php ./vendor/bin/pest
