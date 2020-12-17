#!/usr/bin/env python3

from http.server import HTTPServer, SimpleHTTPRequestHandler, HTTPStatus
import sys
from argparse import ArgumentParser
import json
import time
import logging
import os
from functools import partial


__version__ = "0.0"


WEBSERVER_WORKING_DIR = os.environ.get("WEBSERVER_WORKING_DIR", os.path.join(os.getcwd(), "data"))
os.makedirs(WEBSERVER_WORKING_DIR, exist_ok=True)
WEBSERVER_LOG = os.environ.get("WEBSERVER_LOG", os.path.join(
    WEBSERVER_WORKING_DIR, "webserver.log"))

log_fmt = "%(asctime)s %(levelname)s %(filename)s:%(lineno)d: %(message)s"
logging.basicConfig(
    filename=WEBSERVER_LOG,
    level=logging.INFO,
    format=log_fmt
)
log = logging.getLogger(__name__)
sh = logging.StreamHandler()
sh.setFormatter(logging.Formatter(log_fmt))
log.addHandler(sh)

class RequestHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def handle_error(self, status, message):
        log.error("HTTP %d: %s", status, message)
        self.send_error(HTTPStatus.NOT_IMPLEMENTED, message)

    def do_POST(self):
        ctype = self.headers.get("content-type")
        if ctype != "application/json":
            self.handle_error(HTTPStatus.NOT_IMPLEMENTED,
                              "unsupported content-type: {0}".format(ctype))
            return

        if self.headers.get("content-length", 0) == 0:
            self.handle_error(HTTPStatus.NO_CONTENT, "body is empty")
            return

        lbuf = 1024
        left = int(self.headers["Content-Length"])
        while left > 0:
            if lbuf > left:
                lbuf = left
            data = self.rfile.read(lbuf)
            left -= len(data)

        path = os.path.join(WEBSERVER_WORKING_DIR, time.strftime("%s"))
        try:
            with open(path, "w") as fd:
                json.dump(data.decode(), fd)
            log.info("POST saved: %s", path)
        except Exception as e:
            log.exception("error")
            self.handle_error(HTTPStatus.INTERNAL_SERVER_ERROR,
                              "saving data to server failed: {0}".format(e))

        self.send_response(
            HTTPStatus.OK, "data saved on server: {0}".format(path))
        self.flush_headers()


if __name__ == '__main__':
    parser = ArgumentParser(
        prog="Webserver CLI for serving remote temperature and humidity controller data.")
    parser.add_argument(
        "--port", default=3838, type=int, help="Port to listen on.")
    parser.add_argument(
        "--working-dir", action="store_true", default="./data", help="Directory for serving results."
    )
    args = parser.parse_args(sys.argv[1:])

    address = ("", args.port)
    server = HTTPServer(address, partial(
        RequestHandler))
    host, port = server.server_address
    log.info(
        "Server running on http://%s:%d at %s", host, port, args.working_dir)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
