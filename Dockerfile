FROM malusevd99/php-ext-dev:8.1

ARG CPP_DRIVER_VERSION=2.16.2

ENV EXT_CASSANDRA_VERSION=master
ENV LDFLAGS="-L/usr/local/lib"
ENV LIBS="-lssl -lz -luv -lm -lgmp -lstdc++"
ENV JAVA_HOME=/usr/lib/jvm/default-java
ENV CASSANDRA_HOST=cassandra

RUN php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');" && \
    php composer-setup.php && php -r "unlink('composer-setup.php');" \
    mv composer.phar /bin/composer

COPY --from=mlocati/php-extension-installer /usr/bin/install-php-extensions /usr/local/bin

RUN docker-php-source extract \
    && apt-get update -y \
    && apt-get upgrade -y \
    && mkdir -p /cpp-driver \
    && apt-get install \
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
        python3 \
        python3-pip \
        python3-distutils \
        cppcheck \
        flawfinder \
        psutils \
        iputils-ping \
        libpcre3-dev \
        default-jdk -y \
    && git clone --recursive https://github.com/datastax/cpp-driver /cpp-driver \
    && cd /cpp-driver && git checkout tags/$CPP_DRIVER_VERSION -b v$CPP_DRIVER_VERSION \
    && mkdir -p build && cd build \
    && cmake \
        -DCMAKE_CXX_FLAGS="-fPIC" \
        -DCASS_BUILD_STATIC=ON \
        -DCASS_BUILD_SHARED=ON \
        -DCMAKE_BUILD_TYPE=RELEASE \
        -DCMAKE_INSTALL_LIBDIR:PATH=lib \
        -DCASS_USE_ZLIB=ON .. \
    && make -j8 && make install \
    && cd / && rm -rf /cpp-driver \
    && install-php-extensions intl zip pcntl gmp ast xdebug

RUN mkdir -p /ccm \
    && git clone https://github.com/riptano/ccm.git /ccm \
    && cd /ccm && python3 setup.py install \
    && rm -rf /ccm

RUN pip3 install lizard \
    && pip3 install six \
    && pip3 install psutil

RUN pecl install openswoole

CMD ["bash"]
