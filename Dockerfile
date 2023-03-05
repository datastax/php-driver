ARG PHP_IMAGE=php:8.2-cli

FROM $PHP_IMAGE as base

ARG API_VERSION=20220829
ARG PHP_CONF_DIR=/usr/local/etc/php/conf.d
ARG PHP_EXT_DIR=/usr/local/lib/php/extensions/no-debug-non-zts-${API_VERSION}

COPY --from=mlocati/php-extension-installer /usr/bin/install-php-extensions /usr/local/bin

COPY . /ext-scylladb

WORKDIR /ext-scylladb

RUN apt update -y \
    && apt upgrade -y \
    && apt install -y \
        python3 \
        python3-pip \
        unzip \
        mlocate \
        build-essential \
        ninja-build \
        libssl-dev \
        libgmp-dev \
        zlib1g-dev \
        openssl \
        libpcre3-dev \
    && pip3 install cmake \
    && install-php-extensions \
        intl \
        zip \
        pcntl \
        gmp \
        @composer \
    && apt-get clean

FROM base as build

RUN phpize \
    && cmake --preset Release \
    && cd out/Release \
    && ninja \
    && ninja install \
    && cp cassandra.so ${PHP_EXT_DIR}/cassandra.so \
    && cp ../../cassandra.ini ${PHP_CONF_DIR}/cassandra.ini
