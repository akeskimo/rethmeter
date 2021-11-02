from django.db import models
import uuid
from django.contrib.postgres.fields import ArrayField


MAX_MEASUREMENTS_ARRAY_SIZE = 365 * 24 * 60 * 60


class Device(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, help_text="Device unique registration id.")
    name = models.CharField(max_length=32, help_text="Name of the device.")
    description = models.TextField(max_length=1000, help_text="Description of the device.")
    created = models.DateTimeField(auto_now_add=True, help_text="Registration date.")

    def __str__(self) -> str:
        return f"Device(name={self.name}, id={self.id})"


class Sensor(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, help_text="Sensor unique id.")
    name = models.CharField(max_length=32, help_text="Sensor name.")
    parent = models.OneToOneField(Device, on_delete=models.PROTECT, help_text="Parent device of the sensor.")

    def __str__(self) -> str:
        return f"Sensor(name={self.name}, parent={self.parent})"


class Measurement(models.Model):
    date = models.DateTimeField(auto_now_add=True, help_text="Measurement creation date.")
    value = models.IntegerField(help_text="Measurement value.")
    unit = models.CharField(max_length=16, help_text="Measurement unit.")
    sensor = models.ForeignKey(Sensor, on_delete=models.PROTECT, help_text="Measurement source.")

    class Meta:
        ordering = ['date']

    def __str__(self) -> str:
        return f"Measurement(date={self.date}, value={self.value}, unit={self.unit})"
