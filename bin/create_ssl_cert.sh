#!/usr/bin/env bash

set -o errexit
set -o nounset
set -o pipefail

# Create self-signed certificate to enable application encryption.
# Documentation: https://devcenter.heroku.com/articles/ssl-certificate-self

# Change to working directory.
out="${1-}"
if [[ -z "$out" ]]; then
  echo "missing argument: output directory"
  exit 1
fi

if [[ ! -d "$out" ]]; then
  echo "invalid argument: directory does not exist:" $out
  exit 1
fi

cd $out

openssl genrsa -des3 -passout pass:x -out server.pass.key 2048
openssl rsa -passin pass:x -in server.pass.key -out server.key
rm server.pass.key
openssl req -new -key server.key -out server.csr
openssl x509 -req -sha256 -days 365 -in server.csr -signkey server.key -out server.crt

# Create Diff-Hellman coefficients to prevent Logjam attacks (TLS <1.2).
openssl dhparam -out dhparam.pem 2048
