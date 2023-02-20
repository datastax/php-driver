FROM php:8.2-cli

ARG CPP_DRIVER_VERSION=2.16.2

ENV LDFLAGS="-L/usr/local/lib"
ENV LIBS="-lssl -lz -luv -lm -lgmp -lstdc++"

COPY --from=mlocati/php-extension-installer /usr/bin/install-php-extensions /usr/local/bin

RUN php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"  \
    && php composer-setup.php \
    && php -r "unlink('composer-setup.php');" \
    && mv composer.phar /bin/composer \
    && docker-php-source extract \
    && apt update -y \
    && mkdir -p /cpp-driver \
    && apt install \
        cmake \
        unzip \
        mlocate \
        build-essential \
        ninja-build \
        git \
        libuv1-dev \
        libssl-dev \
        libgmp-dev \
        zlib1g-dev \
        openssl \
        libpcre3-dev -y \
    && git clone --recursive https://github.com/datastax/cpp-driver /cpp-driver \
    && cd /cpp-driver && git checkout tags/$CPP_DRIVER_VERSION -b v$CPP_DRIVER_VERSION \
    && mkdir -p build && cd build \
    && cmake -G Ninja \
      -DCASS_BUILD_STATIC=ON \
      -DCASS_BUILD_SHARED=ON \
      -DCASS_USE_STD_ATOMIC=ON \
      -DCASS_USE_TIMERFD=ON \
      -DCASS_USE_LIBSSH2=ON \
      -DCMAKE_CXX_FLAGS="-O3 -fPIC" \
      -DCMAKE_C_FLAGS="-O3 -fPIC" \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -DCASS_USE_ZLIB=ON .. \
    && ninja && ninja install \
    && install-php-extensions intl zip pcntl gmp xdebug \
    && docker-php-source delete \
    && apt-get clean \
    && rm -rf /cpp-driver
