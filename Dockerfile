FROM malusevd99/php-ext-dev:8.1

ARG CPP_DRIVER_VERSION=2.16.2

ENV EXT_CASSANDRA_VERSION=master
ENV LDFLAGS="-L/usr/local/lib"
ENV LIBS="-lssl -lz -luv -lm -lgmp -lstdc++"

RUN php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');" && \
    php composer-setup.php && php -r "unlink('composer-setup.php');" \
    mv composer.phar /bin/composer

COPY --from=mlocati/php-extension-installer /usr/bin/install-php-extensions /usr/local/bin

RUN docker-php-source extract \
    && apt update -y \
    && mkdir -p /cpp-driver \
    && apt install \
        cmake \
        unzip \
        mlocate \
        build-essential \
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
    && cmake \
        -DCMAKE_CXX_FLAGS="-fPIC" \
        -DCASS_BUILD_STATIC=OFF \
        -DCASS_BUILD_SHARED=ON \
        -DCMAKE_BUILD_TYPE=RELEASE \
        -DCMAKE_INSTALL_LIBDIR:PATH=lib \
        -DCASS_USE_ZLIB=ON .. \
    && make -j8 && make install \
    && install-php-extensions intl zip pcntl gmp ast xdebug

CMD ["bash"]
