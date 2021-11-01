DOCKER_WEBSERVER_IMAGE_NAME=akeskimo/docker-registry:latest

build:
	docker build -t $(DOCKER_WEBSERVER_IMAGE_NAME) -f $(CWD)/webserver/Dockerfile .

push:
	docker push $(DOCKER_WEBSERVER_IMAGE_NAME)

.PHONY: push build
