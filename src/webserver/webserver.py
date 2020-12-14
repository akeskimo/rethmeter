#!/usr/bin/env python3

from flask import Flask
import sys
from argparse import ArgumentParser


app = Flask(__name__)


@app.route('/')
def index():
    return "You are running python {0}".format(sys.version)


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
