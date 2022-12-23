from django.urls import path, include
from .views import QuerySensors
urlpatterns = [
    path("sensors/", QuerySensors.as_view()),
]
