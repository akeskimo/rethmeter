# Webserver

# Install Python Virtual Environment

For local development, you are recommended to use virtual python environment. Using [Pipenv](#Pipenv) is recommended.

## Pipenv
Pipenv keeps static lock file that tracks all packages and including their dependencies keeping
consistent dependency graph across installed packages.

`pipenv install -r requirements.txt`

In order to add new packages in the installed requirements file, you will need to update requirements:

`make update-requirements`

# Run Webserver
To launch webserver on the local environment, run

`python3 django/rethmeter/manage.py runserver localhost:8000`

To access UI, open your browser with link:

http://localhost:8000
