FROM php:8.1
WORKDIR /tmp/cassandra-php-driver

RUN apt update -y \
 && apt install python3 pip cmake unzip mlocate build-essential git libuv1-dev libssl-dev libgmp-dev openssl zlib1g-dev libpcre3-dev openjdk-11-jre openjdk-11-jdk -y \
 && pip install git+https://github.com/riptano/ccm.git@master

COPY --from=mlocati/php-extension-installer /usr/bin/install-php-extensions /usr/local/bin
RUN docker-php-source extract \
 && install-php-extensions @composer intl zip pcntl gmp ast xdebug yaml

COPY lib lib
RUN cmake -DCMAKE_CXX_FLAGS="-fPIC" -DCASS_BUILD_STATIC=OFF -DCASS_BUILD_SHARED=ON -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_LIBDIR:PATH=lib -DCASS_USE_ZLIB=ON lib/cpp-driver \
 && make -j$(nproc) \
 && make install

RUN docker-php-source extract

COPY ext ext
ENV NO_INTERACTION true
RUN cd ext \
 && phpize \
 && LDFLAGS="-L/usr/local/lib" LIBS="-lssl -lz -luv -lm -lgmp -lstdc++" ./configure --with-cassandra=/usr/local \
 && make -j$(nproc) \
 && make test \
 && make install \
 && mv cassandra.ini /usr/local/etc/php/conf.d/docker-php-ext-cassandra.ini \
 && cd ..

RUN ext/doc/generate_doc.sh

COPY composer.json .
RUN composer install -n

COPY support support
COPY tests tests
COPY phpunit.xml .
ENV JAVA_HOME /usr/lib/jvm/java-11-openjdk-amd64/
RUN bin/phpunit --stop-on-error --stop-on-failure

COPY features features
COPY behat.yml .
RUN bin/behat --stop-on-failure --tags="~@skip-ci"

RUN make clean \
 && make clean -C ext

CMD ["bash"]
