LDFLAGS ?= -L/usr/local/lib
LIBS ?= -lssl -lz -luv -lm -lgmp -lstdc++

.PHONY: build
build:
	@cd ext && mkdir cmake_build && cd cmake_build && cmake .. && make -j8

install:
	@cd ext/cmake_build && make install

.PHONY: docker-dev-image
docker-dev-image:
	@docker build \
		--build-arg "IMAGE=malusevd99/php-ext-dev:8.1-debug" \
		-t "ghcr.io/nano-interactive/cassandra-php-driver:dev" \
		--target dev \
		--compress .

.PHONY: docker-production-image
docker-production-image:
	@docker build \
		--build-arg "IMAGE=malusevd99/php-ext-dev:8.1" \
		-t "ghcr.io/nano-interactive/cassandra-php-driver:dev" \
		--target dev \
		--compress .

