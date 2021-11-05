FROM busybox:1.34.1-glibc

ADD docker/broker/files/broker /broker

CMD [ "/broker", "-host", "0.0.0.0", "-port", "4321", "-broker-url", "http://webserver:8000/devices/api/measurement" ]
