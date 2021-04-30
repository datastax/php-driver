FROM debian:buster-slim AS cassandra_cpp_driver

# Datastax/Cassandra CPP Driver
WORKDIR /tmp

RUN echo "deb http://deb.debian.org/debian buster-backports main" > /etc/apt/sources.list.d/backports.list
RUN apt-get update

RUN apt-get -y install g++
RUN apt-get -y install make
RUN apt-get -y install cmake
RUN apt-get -y install libuv1-dev
RUN apt-get -y install libssl-dev
RUN apt-get -y install libgmp-dev
RUN apt-get -y install openssl
RUN apt-get -y install libpcre3-dev
RUN apt-get -y install git
RUN apt-get -y install checkinstall
RUN apt-get -y install libz-dev

RUN git clone https://github.com/datastax/cpp-driver.git \
    && cd cpp-driver \
    && git checkout tags/2.16.0 \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make

RUN cd /tmp/cpp-driver/build \
    && checkinstall -y -D --install=no --pkgname=cassandra-cpp-driver --pkgversion=2.16.0 --pkgarch=amd64

FROM debian:buster-slim

COPY --from=cassandra_cpp_driver /tmp/cpp-driver/build/cassandra-cpp-driver_2.16.0-1_amd64.deb /tmp/cpp-driver/build/cassandra-cpp-driver_2.16.0-1_amd64.deb
RUN dpkg -i /tmp/cpp-driver/build/cassandra-cpp-driver_2.16.0-1_amd64.deb

WORKDIR /tmp
RUN apt-get update
# PHP
RUN apt-get install -y php php-fpm
RUN apt-get install -y php-pear
RUN apt-get install -y php-dev

# Cassandra PHP Driver - PECL Installation
RUN apt-get install -y git
RUN mkdir -p /tmp/php-driver
COPY . /tmp/php-driver/

# Install from git source
#RUN git clone https://github.com/datastax/php-driver.git \
#    && cd /tmp/php-driver \
#    && rm -R /tmp/php-driver/.git

RUN apt-get install -y libgmp-dev
RUN apt-get install -y libuv1-dev
RUN cd /tmp/php-driver \
    && pecl install ext/package.xml \
    && rm -R /tmp/*