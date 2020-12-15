#!/usr/bin/env bash

# Create self-signed certificate to enable application encryption.
# Documentation: https://devcenter.heroku.com/articles/ssl-certificate-self

# Change to working directory if given.
if [ -d $1 ]; then
  cd $1
fi

openssl genrsa -des3 -passout pass:x -out server.pass.key 2048
openssl rsa -passin pass:x -in server.pass.key -out server.key
rm server.pass.key
openssl req -new -key server.key -out server.csr
openssl x509 -req -sha256 -days 365 -in server.csr -signkey server.key -out server.crt

# Create Diff-Hellman coefficients to prevent Logjam attacks (TLS <1.2).
openssl dhparam -out dhparam.pem 2048
