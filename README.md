# rethmeter
Remote temperature and humidity meter controller with visualizer.

# Requirements
The project builds are containerized and deployed on docker images to make the code
more portable. To install docker, follow the instructions on link below:

https://docs.docker.com/get-docker/

# First Time Setup

## SSL/TLS
The webserver will require certificate in secrets folder for secure connections.
If you do not have certificates provided by authority, you may create your own with

`make create-ssl-certificates`

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
This will start the containerized webserver. If this is the first time that you are
launching the webserver, see #first-time-setup section.

`make run-webserver`

To access UI, open your browser with link:

https://localhost

# Contributing
To access repository you may ask a project admin by PM.
