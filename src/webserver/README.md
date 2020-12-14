# Webserver

# Install Python Virtual Environment

For local development, you are recommended to use virtual python environment, see `pyenv` or `virtualenv`.

## Pyenv
Pyenv allows managing multiple python versions which gives developer flexibility to choose between different
python versions without needing to upgrade the system python.

### Install
`setup/linux/pyenv_init.sh`

### Usage
Install python version 3.5.3, activate it and install pip packages on it.

```bash
pyenv install 3.5.3
pyenv virtualenv 3.5.3 rethmeter-webserver
pyenv virtualenv activate rethmeter-webserver
pip install -r requirements.txt
```

## Virtualenv
If you prefer to use your system-wide python or are old-school, you can use virtualenv, for example.

### Install
`apt-get install virtualenv`

### Usage
```bash
virtualenv --python python3 env
source env/bin/activate
pip install -r requirements.txt
```

# Run Webserver
To launch webserver on the local environment, run

`./webserver`

To access UI, open your browser with link:

http://localhost:3838
