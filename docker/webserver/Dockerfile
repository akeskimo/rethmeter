FROM debian:stretch

ARG WEBSERVER_PORT=3838

# Install system level dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update
RUN apt-get -y install \
    curl git python3 python3-pip

ADD src/webserver /srv/webserver
WORKDIR /srv/webserver
RUN pip3 install -r requirements.txt

CMD [ "python3", "django/rethmeter/manage.py", "runserver", "localhost:8000" ]
