import json
import logging
import os
from json.decoder import JSONDecodeError
from django.http.request import HttpRequest
from jsonschema.exceptions import ValidationError
from jsonschema import validate, Draft7Validator
from django.http.response import HttpResponse, HttpResponseNotAllowed, HttpResponseServerError, JsonResponse
from django.http import HttpResponseBadRequest
from django.shortcuts import render
from django.views import generic
from django.views.decorators.csrf import csrf_exempt

from .models import Device, Sensor, Measurement


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


class JSONResponse(JsonResponse):
    def __init__(self, message: str, status: int, **kwargs) -> None:
        data = {"message": message, "code": status}
        super().__init__(data=data, status=status, **kwargs)


@csrf_exempt
def api_sensor_data(request: HttpRequest) -> JSONResponse:
    try:
        data = json.loads(request.body.decode())
    except JSONDecodeError as e:
        return JSONResponse(message="Decoding of JSON failed: %s" % e, status=HttpResponseBadRequest.status_code)
    except Exception as e:
        return JSONResponse(message="Unhandled error when decoding JSON: %s" % e, status=HttpResponseBadRequest.status_code)

    try:
        with open(MEASUREMENT_SCHEMA_FILE, "r") as fh:
            try:
                schema = json.load(fh)
            except JSONDecodeError:
                return JSONResponse(message="Decoding schema file failed. Check server logs.", status=HttpResponseServerError.status_code)
            try:
                validate(data, schema, cls=Draft7Validator)
            except ValidationError as e:
                logger.error("Measurement POST request rejected: Validation error: %s", e.message)
                return JSONResponse(message="Invalid format: %s" % e.message, status=HttpResponseBadRequest.status_code)
    except Exception as e:
        logger.exception("Unhandled error while validating request body:")
        return JSONResponse(message="Unexpected error while validating request body. Check server logs.", status=HttpResponseBadRequest.status_code)

    return JSONResponse(message="SUCCESS", status=HttpResponse.status_code)
