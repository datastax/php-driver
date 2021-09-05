FROM php:8.0

ENV EXT_CASSANDRA_VERSION=master

RUN php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');" && \
    php composer-setup.php && php -r "unlink('composer-setup.php');" \
    mv composer.phar /bin/composer

COPY --from=mlocati/php-extension-installer /usr/bin/install-php-extensions /usr/local/bin

RUN docker-php-source extract \
    && apt update -y \
    && apt install python3 pip cmake unzip mlocate build-essential git libuv1-dev libssl-dev libgmp-dev openssl zlib1g-dev libpcre3-dev -y \
    && git clone --branch $EXT_CASSANDRA_VERSION --depth 1 https://github.com/nano-interactive/php-driver.git /usr/src/php/ext/cassandra \
    && cd /usr/src/php/ext/cassandra && git submodule update --init \
    && mkdir -p /usr/src/php/ext/cassandra/lib/cpp-driver/build \
    && cmake -DCMAKE_CXX_FLAGS="-fPIC" -DCASS_BUILD_STATIC=OFF -DCASS_BUILD_SHARED=ON -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_LIBDIR:PATH=lib -DCASS_USE_ZLIB=ON /usr/src/php/ext/cassandra/lib/cpp-driver \
    && make -j8 \
    && make install \
    && install-php-extensions intl zip pcntl gmp ast xdebug

RUN cd /usr/src/php/ext/cassandra/ext \
    && phpize \
    && LDFLAGS="-L/usr/local/lib" LIBS="-lssl -lz -luv -lm -lgmp -lstdc++" ./configure --with-cassandra=/usr/local \
    && make -j8 && make install && updatedb && pip install ccm


CMD ["bash"]
