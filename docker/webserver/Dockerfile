FROM debian:stretch

# Install system level dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update
RUN apt-get -y install \
    curl git python3 python3-pip

# Install python dependencies.
COPY src/webserver/requirements.txt /app/requirements.txt
WORKDIR /app
RUN pip3 install -r requirements.txt

CMD [ "python3", "django/rethmeter/manage.py", "runserver", "0.0.0.0:8000" ]
