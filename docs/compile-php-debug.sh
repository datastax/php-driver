#!/usr/bin/env bash
# -*- coding: utf-8 -*-

# This script is used to compile PHP with debug symbols.

PHP_VERSION=$1
PHP_ZTS=$2
OUTPUT=$1

CFLAGS="-g -ggdb"
CXXFLAGS="-g -ggdb"

if [[ -z "$PHP_ZTS" ]]; then
    PHP_ZTS="no"
fi

if [[ -z "$OUTPUT" ]]; then
    OUTPUT="$HOME"
fi

if [[ -z "$PHP_VERSION" ]]; then
    echo "Usage: compile-debug-php.sh PHP_VERSION [ZTS=yes|no]"
    exit 1
fi

grep "NAME=\"Ubuntu\"" "/etc/os-release" >>/dev/null
if [[ $? == 0 ]]; then
    sudo apt-get install \
        libssl-dev \
        libsqlite3-dev \
        zlib1g-dev \
        libcurl4-openssl-dev \
        libreadline-dev \
        libffi-dev \
        libonig-dev \
        libbz2-dev \
        libsodium-dev \
        libpq-dev \
        libzip-dev -y
fi

PHP_SOURCE_CODE="https://github.com/php/php-src/archive/refs/tags/php-$PHP_VERSION.tar.gz"

# Download PHP source code
wget -O "php-$PHP_VERSION.tar.gz" "$PHP_SOURCE_CODE"

# Extract PHP source code
tar -C "/tmp/php-$PHP_VERSION-src" -xzf "php-$PHP_VERSION.tar.gz"

rm -rf "$OUTPUT/php/$PHP_VERSION-$PHP_MODE-nts"

grep "NAME=\"Ubuntu\"" "/etc/os-release" >>/dev/null
if [[ $? == 0 ]]; then
    sudo update-alternatives --install /bin/php-cgi php-cgi "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/php-cgi" 1
    sudo update-alternatives --install /bin/php-config php-config "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/php-config" 1
    sudo update-alternatives --install /bin/phpize phpize "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/phpize" 1
    sudo update-alternatives --install /bin/php php "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/php" 1
    sudo update-alternatives --install /bin/phpdbg phpdbg "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/phpdbg" 1
fi

pushd "/tmpphp-src-php-$PHP_VERSION" || exit 1

./buildconf --force
./configure \
    --enable-debug \
    --prefix="$OUTPUT/php/$PHP_VERSION-$PHP_MODE-nts" \
    --enable-embed=static \
    --enable-opcache \
    --enable-debug \
    --disable-short-tags \
    --with-openssl \
    --with-zlib \
    --with-curl \
    --with-ffi \
    --enable-pcntl \
    --enable-intl \
    --enable-sockets \
    --with-zip \
    --with-pic \
    --with-readline \
    --enable-mbstring \
    --with-sqlite3 \
    --enable-fpm \
    --enable-calendar \
    --enable-bcmath \
    --with-bz2 \
    --with-gmp \
    --with-gettext \
    --with-mysqli \
    --with-pgsql \
    --with-sodium \
    --with-pear \
    --with-pcre-regex

make -j"$(nproc)"
make install

if [[ "$PHP_ZTS" == "yes" ]]; then
    rm -rf "$OUTPUT/php/$PHP_VERSION-debug-zts"

    grep "NAME=\"Ubuntu\"" "/etc/os-release" >>/dev/null
    if [[ $? == 0 ]]; then
        sudo update-alternatives --install /bin/php-cgi php-cgi "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/php-cgi" 1
        sudo update-alternatives --install /bin/php-config php-config "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/php-config" 1
        sudo update-alternatives --install /bin/phpize phpize "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/phpize" 1
        sudo update-alternatives --install /bin/php php "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/php" 1
        sudo update-alternatives --install /bin/phpdbg phpdbg "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/phpdbg" 1
    fi

    make clean && git clean -X -f -d && ./buildconf --force
    ./configure \
        CFLAGS="$CFLAGS" \
        CXXFLAGS="$CXXFLAS" \
        --prefix="$OUTPUT/php/$PHP_VERSION-debug-zts" \
        --enable-embed=static \
        --enable-zts \
        --enable-phpdbg \
        --enable-opcache \
        --enable-debug \
        --disable-short-tags \
        --with-openssl \
        --with-zlib \
        --with-curl \
        --with-ffi \
        --enable-pcntl \
        --enable-socket \
        --with-zip \
        --with-pic \
        --with-readline \
        --enable-mbstring \
        --with-sqlite3 \
        --enable-fpm \
        --enable-calendar \
        --enable-bcmath \
        --with-bz2 \
        --with-gmp \
        --with-gettext \
        --with-mysqli \
        --with-pgsql \
        --with-sodium \
        --with-pear
fi

make -j"$(nproc)"
make install
popd || exit 1

grep "NAME=\"Ubuntu\"" "/etc/os-release" >>/dev/null
if [[ $? == 0 ]]; then
    sudo update-alternatives --install /bin/php-cgi php-cgi "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/php-cgi" 1
    sudo update-alternatives --install /bin/php-cgi php-cgi "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/php-cgi" 1

    sudo update-alternatives --install /bin/php-config php-config "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/php-config" 1
    sudo update-alternatives --install /bin/php-config php-config "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/php-config" 1

    sudo update-alternatives --install /bin/phpize phpize "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/phpize" 1
    sudo update-alternatives --install /bin/phpize phpize "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/phpize" 1

    sudo update-alternatives --install /bin/php php "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/php" 1
    sudo update-alternatives --install /bin/php php "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/php" 1

    sudo update-alternatives --install /bin/phpdbg phpdbg "$OUTPUT/php/$PHP_VERSION-debug-nts/bin/phpdbg" 1
    sudo update-alternatives --install /bin/phpdbg phpdbg "$OUTPUT/php/$PHP_VERSION-debug-zts/bin/phpdbg" 1
fi
