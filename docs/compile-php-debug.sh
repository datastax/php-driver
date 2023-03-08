#!/usr/bin/env bash
# -*- coding: utf-8 -*-

CFLAGS="-g -ggdb"
CXXFLAGS="-g -ggdb"
IS_UBUNTU=0

print_usage() {
    echo ""
    echo "Usage: compile-debug-php.sh [OPTION] [ARG]"
    echo "-v ARG php version"
    echo "-o ARG output path, default: $HOME"
    echo "-z Use ZTS"
    echo ""
}

check_ubuntu() {
    if grep "NAME=\"Ubuntu\"" "/etc/os-release" >>/dev/null; then
        IS_UBUNTU=1
    fi
}

compile_php() {
    local ZTS="$1"

    local config=(
        --enable-embed=static
        --enable-phpdbg
        --enable-opcache
        --enable-debug
        --disable-short-tags
        --with-openssl
        --with-zlib
        --with-curl
        --with-ffi
        --enable-pcntl
        --enable-socket
        --with-zip
        --with-pic
        --with-readline
        --enable-mbstring
        --with-sqlite3
        --enable-fpm
        --enable-calendar
        --enable-bcmath
        --with-bz2
        --with-gmp
        --with-gettext
        --with-mysqli
        --with-pgsql
        --with-sodium
        --with-pear
    )

    if [[ "$ZTS" == "yes" ]]; then
        local OUTPUT_PATH="$OUTPUT/php/$PHP_BASE_VERSION-debug-zts"
        config+=("--enable-zts")
    else
        local OUTPUT_PATH="$OUTPUT/php/$PHP_BASE_VERSION-debug-nts"
    fi

    rm -rf "$OUTPUT_PATH"

    wget -O "php-$PHP_VERSION.tar.gz" "https://github.com/php/php-src/archive/refs/tags/php-$PHP_VERSION.tar.gz"
    tar -C "/tmp" -xzf "php-$PHP_VERSION.tar.gz"

    rm -f "php-$PHP_VERSION.tar.gz"

    if [ "$IS_UBUNTU" -eq 1 ]; then
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

        sudo update-alternatives --force --remove php-cgi "$OUTPUT_PATH/bin/php-cgi"
        sudo update-alternatives --force --remove php-config "$OUTPUT_PATH/bin/php-config"
        sudo update-alternatives --force --remove phpize "$OUTPUT_PATH/bin/phpize"
        sudo update-alternatives --force --remove php "$OUTPUT_PATH/bin/php"
        sudo update-alternatives --force --remove phpdbg "$OUTPUT_PATH/bin/phpdbg"
    fi

    pushd "/tmp/php-src-php-$PHP_VERSION" || exit 1
    ./buildconf --force
    ./configure CFLAGS="$CFLAGS" CXXFLAGS="$CXXFLAGS" --prefix="$OUTPUT_PATH" "${config[@]}"
    make "-j$(nproc)"
    make install
    popd || exit 1

    rm -rf "/tmp/php-src-php-$PHP_VERSION"

    if [ "$IS_UBUNTU" -eq 1 ]; then
        sudo update-alternatives --install /bin/php-cgi php-cgi "$OUTPUT_PATH/bin/php-cgi" 1
        sudo update-alternatives --install /bin/php-config php-config "$OUTPUT_PATH/bin/php-config" 1
        sudo update-alternatives --install /bin/phpize phpize "$OUTPUT_PATH/bin/phpize" 1
        sudo update-alternatives --install /bin/php php "$OUTPUT_PATH/bin/php" 1
        sudo update-alternatives --install /bin/phpdbg phpdbg "$OUTPUT_PATH/bin/phpdbg" 1
    fi
}

check_deps() {
    deps="wget make git"

    for dep in $deps; do
        [ -z "$(command -v "$dep")" ] && echo "Unsatisfied dependency: $dep" && exit 1
    done
}

check_deps && check_ubuntu

while getopts "v:zo:" option; do
    case "$option" in
    "v") PHP_VERSION="$OPTARG" ;;
    "z") PHP_ZTS="yes" ;;
    "o") OUTPUT="$OPTARG" ;;
    *) print_usage ;;
    esac
done

if [[ -z "$PHP_ZTS" ]]; then
    PHP_ZTS="no"
fi

if [[ -z "$OUTPUT" ]]; then
    OUTPUT="$HOME"
fi

if [[ -z "$PHP_VERSION" ]]; then
    print_usage
    exit 1
fi

PHP_BASE_VERSION=$(echo "$PHP_VERSION" | cut -d. -f1,2)

compile_php "no"

if [[ $PHP_ZTS == "yes" ]]; then
    compile_php "yes"
fi
