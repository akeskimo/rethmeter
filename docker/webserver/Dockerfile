FROM python:3.9.7-bullseye

# Install system level dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y install \
    curl git

# Install python dependencies.
COPY src/webserver/requirements.txt /app/requirements.txt
WORKDIR /app
RUN pip3 install -r requirements.txt
RUN pip3 install ipython

CMD [ "python3", "django/rethmeter/manage.py", "runserver", "0.0.0.0:8000" ]
