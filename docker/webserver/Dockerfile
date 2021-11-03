FROM python:3.8-slim as base

FROM base as builder

RUN mkdir /install
WORKDIR /install

ADD src/webserver/requirements.txt requirements.txt
RUN pip3 install --prefix=/install configparser
RUN pip3 install --prefix=/install -r requirements.txt

RUN pip3 install --prefix=/install ipython

FROM base

COPY --from=builder /install /usr/local

CMD [ "python3", "django/rethmeter/manage.py", "runserver", "0.0.0.0:8000" ]
