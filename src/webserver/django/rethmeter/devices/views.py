from json.decoder import JSONDecodeError
from django.http.response import HttpResponse, HttpResponseNotAllowed, HttpResponseServerError
from django.shortcuts import render
from jsonschema.exceptions import ValidationError
from .models import Device, Sensor, Measurement
from django.views import generic
from django.http import HttpResponseBadRequest
import json
import logging
from jsonschema import validate, Draft7Validator
import os
from django.views.decorators.csrf import csrf_exempt


logger = logging.getLogger(__name__)


MEASUREMENT_SCHEMA_FILE = os.path.join(os.path.dirname(__file__), "schemas", "measurement.json")


class SensorListView(generic.ListView):
    model = Sensor
    template_name = 'sensors.html'


def index(request):
    context = {}

    devices = Device.objects.all()

    context['devices'] = [
        {
            'uuid': d.id,
            'name': d.name,
            'sensors': []
        } for d in devices or []
    ]

    for device in context['devices'] or []:
        sensors = Sensor.objects.filter(parent__exact=device['uuid'])

        device['sensors'] = [
            {
                'uuid': m.id,
                'num_measurements': Measurement.objects.filter(sensor__exact=m.id).count()
            } for m in sensors
        ]

    return render(request, 'index.html', context=context)


@csrf_exempt
def api_sensor_data(request):
    try:
        logger.info("Request body: %s", request.body)
        data = json.loads(request.body.decode())
    except JSONDecodeError as e:
        return HttpResponseBadRequest("Decoding of JSON failed: %s", e)
    except Exception as e:
        return HttpResponseBadRequest("Unhandled error when decoding JSON: %s", e)

    with open(MEASUREMENT_SCHEMA_FILE, "r") as fh:
        try:
            schema = json.load(fh)
        except JSONDecodeError:
            return HttpResponseServerError("Decoding schema file failed. Check server logs.")
        try:
            validate(data, schema, cls=Draft7Validator)
        except ValidationError as e:
            logger.error("Measurement POST request rejected: Validation error: %s", e.message)
            return HttpResponseNotAllowed("Invalid format: %s", e.message)

    logger.info("Received data: %s", data)

    return HttpResponse("SUCCESS")
