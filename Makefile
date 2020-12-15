CWD=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
DOCKER_WEBSERVER_IMAGE_NAME=akeskimo/docker-registry:latest

docker-build-webserver:
	docker build -t $(DOCKER_WEBSERVER_IMAGE_NAME) -f $(CWD)/docker/webserver/Dockerfile .

docker-push-webserver:
	docker push $(DOCKER_WEBSERVER_IMAGE_NAME)

run-webserver:
	docker-compose -f $(CWD)/docker/docker-compose.yml up

create-ssl-certificates:
	$(CWD)/bin/create_ssl_cert.sh secrets