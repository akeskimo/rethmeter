# Webserver

# Install Python Virtual Environment

For local development, you are recommended to use virtual python environment. Using [Pipenv](#Pipenv) is recommended.

## Pipenv
Pipenv keeps static lock file that tracks all packages and including their dependencies keeping
consistent dependency graph across installed packages.

`pipenv install -r requirements.txt`

In order to add new packages in the installed requirements file, you will need to update requirements:

`make update-requirements`

## Django Admin

In order to manage the django database it will be easier by creating a superuser


Login to the webserver docker image:

`docker exec -it webserver bash`

Execute create super user command

`root@d2bc215aafbe:/app# python3 django/rethmeter/manage.py createsuperuser`

...after that you will need to relaunch the site and you should be able to login now

http://localhost/admin

# Initialize database with dummy data

To populate the django database with test data, you can use a python helper script
to get a feel of the UI.

Ensure that you have initialized the devices tables in database:

`docker exec webserver python3 django/rethmeter/manage.py makemigrations devices`
`docker exec webserver python3 django/rethmeter/manage.py migrate`

Populate database with dummy data:

`docker exec -i webserver python3 django/rethmeter/manage.py shell -v 3 < django/rethmeter/populate_db_devices_dummy.py`

NOTE: Running the script in django shell gives access to all django models and views.

# Run Webserver
To launch webserver on the local environment, run

`python3 django/rethmeter/manage.py runserver`

To access UI, open your browser with link:

http://localhost:8000
