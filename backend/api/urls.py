from django.urls import path, include
from .views import QuerySensors,  WriteActuator
urlpatterns = [
    path("read/<str:topic>/", QuerySensors.as_view()),
    path("write/<str:topic>/", WriteActuator.as_view()),
]
