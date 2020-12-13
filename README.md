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

`docker build -t akeskimo/docker-registry:latest`

**Access**
To enter bash prompt on the container, run

`docker run -it --entrypoint=/bin/bash akeskimo/docker-registry:latest`

**Publish**
If you need to update the remote docker image you may publish it with

`docker push akeskimo/docker-registry:latest`

# Get the Code
`git clone https://github.com/akeskimo/rethmeter.git`

# Contributing
To access repository you may ask a project admin by PM.
