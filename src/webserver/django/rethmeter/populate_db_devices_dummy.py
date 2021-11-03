#!/usr/bin/env python3

from devices.models import Device, Sensor, Measurement
import random

device1 = Device(name="DummyDevice1", id="cbb4f156-70ba-453c-ba47-4ec2301b654c", description="Dummy device for displaying fake data.")
device1.save()

sensor11 = Sensor(name="DummySensor1", parent=device1)
sensor11.save()

for _ in range(45):
    value = random.randint(15, 30)
    m = Measurement(value=value, unit="C", sensor=sensor11)
    m.save()


device2 = Device(name="DummyDevice2", id="0bc25db7-c44e-4e94-9305-b377bd5f6e72", description="Dummy device for displaying fake data.")
device2.save()

sensor21 = Sensor(name="DummySensor1", parent=device2)
sensor21.save()

for _ in range(45):
    value = random.randint(-15, 15)
    m = Measurement(value=value, unit="C", sensor=sensor21)
    m.save()

sensor22 = Sensor(name="DummySensor2", parent=device2)
sensor22.save()

for _ in range(45):
    value = random.randint(60, 80)
    m = Measurement(value=value, unit="%", sensor=sensor21)
    m.save()
