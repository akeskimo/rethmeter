FROM debian:stretch

# Install system level dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update
RUN apt-get -y install \
    curl git python3 python3-pip

ADD src/webserver/requirements.txt /
RUN pip3 install -r requirements.txt

ADD docker/webserver/entrypoint.sh /
ENTRYPOINT ["/entrypoint.sh"]
