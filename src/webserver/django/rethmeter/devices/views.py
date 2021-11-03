from django.shortcuts import render
from .models import Device, Sensor, Measurement
from django.views import generic


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
