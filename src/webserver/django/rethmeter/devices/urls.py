from django.urls import path
from . import views


urlpatterns = [
    path('', views.index, name='index'),
    path('sensors', views.SensorListView.as_view(), name='sensors')
]
