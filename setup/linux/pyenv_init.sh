#!/usr/bin/env bash

# Install pyenv python virtualenvironment.
# Documentation: https://github.com/pyenv/pyenv-installer

if [ -d ~/.pyenv ]; then
  echo "pyenv has already been installed. Please remove ~/.pyenv and the environment variables to uninstall."
  exit 1
fi

curl https://pyenv.run | bash

cat << EOF
To enable pyenv in your shell environment, you should add these lines in your shell configuration file, e.g. ~/.bashrc

export PATH=\$PATH:~/.pyenv/bin
eval "\$(pyenv init -)"
eval "\$(pyenv virtualenv-init -)"

After changing the source file, you may reload shell with

exec "\$SHELL"
EOF
