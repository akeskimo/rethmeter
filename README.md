# rethmeter
Remote temperature and humidity meter controller with visualizer.

# Requirements
The project builds are containerized and deployed on docker images to make the code
more portable. To install docker, follow the instructions on link below:

https://docs.docker.com/get-docker/

**Extra**
To download prebuilt images, you will need a Docker Hub account
https://hub.docker.com/signup

# Development

## Docker Help

**Build**
To update or add system dependencies, you will need to rebuild the image by executing

`make docker-build-webserver`

**Debug**
To enter bash prompt on the docker container, run

`docker run -it --entrypoint=/bin/bash akeskimo/docker-registry:latest`

**Publish**
If you need to update the remote docker image you may publish it with

`make docker-push-webserver`

# Get the Code

`git clone https://github.com/akeskimo/rethmeter.git`

# Run Websever

`docker-compose -f docker/docker-compose.yml up`

To access UI, open your browser with link:

https://localhost

# Contributing
To access repository you may ask a project admin by PM.
