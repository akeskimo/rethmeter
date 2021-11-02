CWD=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

first: docker

docker:
	make -C docker build

run-webserver: docker
	docker-compose -f docker/docker-compose.yml up

create-ssl-certificates:
	bin/create_ssl_cert.sh secrets

.PHONY: create-ssl-certificates docker first
