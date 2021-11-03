FROM python:3.9.7-slim-bullseye

# Install python dependencies.
COPY src/webserver/requirements.txt /app/requirements.txt
WORKDIR /app
RUN pip3 install -r requirements.txt
RUN pip3 install ipython

CMD [ "python3", "django/rethmeter/manage.py", "runserver", "0.0.0.0:8000" ]
