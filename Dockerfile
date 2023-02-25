ARG PHP_IMAGE=php:8.2-cli

FROM $PHP_IMAGE as base

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
        python3 \
        python3-pip \
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
    && pip3 install cmake \
    && install-php-extensions intl zip pcntl gmp \
    && docker-php-source delete \
    && apt-get clean \

FROM base as build

RUN mkdir -p build/Release \
    && phpize \
    && cd build/Release \
    && cmake \
      -G "Ninja" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCASS_CPP_STANDARD:STRING=17 \
      -DCASS_USE_STATIC_LIBS:BOOL=ON \
      ../.. \
    && ninja \
    && ninja install \
    && cp cassandra.so ${PHP_EXT_DIR}/cassandra.so \
    && cp ../../cassandra.ini ${PHP_CONF_DIR}/cassandra.ini