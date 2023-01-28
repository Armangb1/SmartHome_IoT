from django.urls import path, include
from .views import QuerySensors, TestApi
urlpatterns = [
    path("sensors/", QuerySensors.as_view()),
    path("test/", TestApi.as_view()),
]
