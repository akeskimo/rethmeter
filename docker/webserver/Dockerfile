FROM debian:stretch

# Install system level dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update
RUN apt-get -y install \
    curl git python3 python3-pip

COPY entrypoint.sh /

ENTRYPOINT ["/entrypoint.sh"]
