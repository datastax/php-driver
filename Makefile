LDFLAGS ?= -L/usr/local/lib
LIBS ?= -lssl -lz -luv -lm -lgmp -lstdc++

all: build copy

.PHONY: build
build:
	cd ext && phpize
	cd ext && ./configure --with-cassandra=/usr/local
	cd ext && make -j8
	cd ext && make install

config:
	cp ./ext/cassandra.ini /usr/local/etc/php/conf.d/cassandra.ini

clean:
	cd ext && $(MAKE) clean
