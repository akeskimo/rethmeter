CWD=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
MKDIR=mkdir -p
DB_DIR=docker/postgres/data/

first:

directories: ${DB_DIR}

${DB_DIR}:
	${MKDIR} ${DB_DIR}

docker:
	$(MAKE) -C docker

run-webserver: docker directories
	docker-compose -f docker/docker-compose.yml up

create-ssl-certificates:
	bin/create_ssl_cert.sh secrets

.PHONY: create-ssl-certificates docker first directories
