#!/usr/bin/env python3

from flask import Flask, request
import sys
from argparse import ArgumentParser


app = Flask(__name__)


@app.route('/')
def index():
    return "You are running python {0}".format(sys.version)


@app.route('/api/post', methods=["POST"])
def post():
    supported_ctypes = ["application/json"]
    if request.content_type not in supported_ctypes:
        return "Unsupported content-type: {0}, supported types: {1}".format(request.content_type, supported_ctypes)
    return "{0} - POST received with data: {1}".format(request.host, request.get_json())


def run_forever(args):
    app.run(debug=args.debug, port=args.port, host="0.0.0.0")


if __name__ == '__main__':
    parser = ArgumentParser(
        prog="Webserver CLI for serving remote temperature and humidity controller data.")
    parser.add_argument(
        "--port", default=3838, type=int, help="Port to listen on")
    parser.add_argument(
        "--debug", action="store_true", help="Enable debugging")
    args = parser.parse_args(sys.argv[1:])

    run_forever(args)
