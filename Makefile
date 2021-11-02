CWD=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

install:
	pipenv install -r src/webserver/requirements.txt

sync:
	pipenv sync

run-webserver:
	docker-compose -f docker/docker-compose.yml up

create-ssl-certificates:
	bin/create_ssl_cert.sh secrets

.PHONY: install sync create-ssl-certificates
