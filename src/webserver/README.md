# Webserver

# Install Python Virtual Environment

For local development, you are recommended to use virtual python environment, see `Pipenv`.

## Pipenv
Pipenv keeps static lock file that tracks all packages and including their dependencies keeping
consistent dependency graph across installed packages.

`pipenv install -r requirements.txt`

To bind the version in updated requirements.txt, you will need to freeze the versions:

`pipenv run pip freeze > requirements.txt`

# Run Webserver
To launch webserver on the local environment, run

`./webserver`

To access UI, open your browser with link:

http://localhost:3838
