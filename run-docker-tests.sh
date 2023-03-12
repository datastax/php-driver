#!/usr/bin/env bash
# -*- coding: utf-8 -*-

docker compose up -d || exit 1

run_tests() {
    local php_version=$1
    docker run \
        --rm \
        -it \
        --network scylladb-php-driver_default \
        -v "$(pwd):/ext-scylladb-php-$php_version" \
        -w "/ext-scylladb-php-$php_version" \
        "malusevd99/scylladb-php-driver:php-$php_version" \
        "/ext-scylladb-php-$php_version/run-tests.sh" || exit 1
}

# run_tests "8.0"
# run_tests "8.0-zts"
run_tests "8.1"
run_tests "8.1-zts"
run_tests "8.2"
run_tests "8.2-zts"
