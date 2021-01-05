#!/usr/bin/env bash

set -o errexit
set -o nounset
set -o pipefail

WEBSERVER_PORT=${WEBSERVER_PORT-}

if [[ -z "$WEBSERVER_PORT" ]]; then
 echo "no webserver port set in environment"
 exit 1
fi

./webserver.py --port $WEBSERVER_PORT