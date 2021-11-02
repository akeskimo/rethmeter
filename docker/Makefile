DOCKER_WEBSERVER_IMAGE_NAME=akeskimo/docker-registry:latest

build:
	docker build -t $(DOCKER_WEBSERVER_IMAGE_NAME) -f webserver/Dockerfile ../

push:
	docker push $(DOCKER_WEBSERVER_IMAGE_NAME)

.PHONY: push build
