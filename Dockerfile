ARG PHP_IMAGE=php:8.2-cli

FROM PHP_IMAGE

ARG API_VERSION=20220829
ARG PHP_CONF_DIR=/usr/local/etc/php/conf.d
ARG PHP_EXT_DIR=/usr/local/lib/php/extensions/no-debug-non-zts-${API_VERSION}

COPY --from=mlocati/php-extension-installer /usr/bin/install-php-extensions /usr/local/bin

COPY . /ext-scylladb

WORKDIR /ext-scylladb

RUN php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"  \
    && php composer-setup.php \
    && php -r "unlink('composer-setup.php');" \
    && mv composer.phar /bin/composer \
    && docker-php-source extract \
    && apt update -y \
    && apt install \
        cmake \
        unzip \
        mlocate \
        build-essential \
        ninja-build \
        git \
        libssl-dev \
        libgmp-dev \
        zlib1g-dev \
        openssl \
        libpcre3-dev -y \
    && install-php-extensions intl zip pcntl gmp xdebug \
    && docker-php-source delete \
    && apt-get clean \
    && mkdir -p build/Release \
    && cd build/Release \
    && cmake \
      -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DPHP_SCYLLADB_LIBUV_STATIC=ON \
      -DPHP_SCYLLADB_LIBSCYLLADB_STATIC=ON \
      ../.. \
    && ninja \
    && ninja install \
    && cp /ext-scylladb/build/Release/cassandra.so ${PHP_EXT_DIR}/scylladb.so \
    && echo "extension=scylladb.so" > ${PHP_CONF_DIR}/scylladb.ini \
