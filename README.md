# rethmeter
Remote temperature and humidity meter controller with visualizer.

# Requirements
In order to make it easier to test and deploy the project the project builds are containerized with
docker which you can install on any platform (Mac OS X, Windows, Linux) on the below link:

Install Docker:
https://docs.docker.com/get-docker/

In order to access prebuilt images, you will need a Docker Hub account
https://hub.docker.com/signup

# Development

## Docker Help

**Build**
To update or add system dependencies, you will need to rebuild the image by executing

`docker build -t akeskimo/docker-registry:latest`

**Access**
To access familiar bash prompt on the container, run

`docker run -it --entrypoint=/bin/bash akeskimo/docker-registry:latest`

**Publish**
If you want to publish the updated docker image you may push the docker image to remote repository

`docker push akeskimo/docker-registry:latest`

# Get the Code
`git clone https://github.com/akeskimo/rethmeter.git`

# Contributing
Ask access to repository by asking a project admin by PM.
