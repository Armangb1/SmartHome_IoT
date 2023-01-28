from django.shortcuts import render

# Create your views here.
from rest_framework.views import APIView
from rest_framework.permissions import IsAdminUser
from rest_framework.response import Response
from config.settings import INFLUXDB_BUCKET, INFLUXDB_TOKEN, INFLUXDB_ORG, INFLUXDB_URL
from influxdb_client import InfluxDBClient
from json import loads


class QuerySensors(APIView):
    """
    this view get's data about sensor from the post request
    and create a influxdb query and get data from database.

    for example:
    for request like this:

        request_data ={
            "sensor": "temprature",
            "time": "-20m",
        } 

    it gets data like this:
    [
    {
        "result": "_result",
        "table": 0,
        "_start": "2022-12-23T19:50:28.262166+00:00",
        "_stop": "2022-12-23T20:40:28.262166+00:00",
        "_time": "2022-12-23T20:16:08.499755+00:00",
        "_value": 25.3,
        "_field": "temperature",
        "_measurement": "my_measurement"
    }
    ]
"""
    permission_classes = [IsAdminUser]

    def post(self, request):

        request_data = request.data

        client = InfluxDBClient(url=INFLUXDB_URL,
                                token=INFLUXDB_TOKEN, org=INFLUXDB_ORG)
        query_api = client.query_api()

        # TODO: add some more featue to the query : measurement
        query = f'from(bucket:"{INFLUXDB_BUCKET}")\
        |> range(start: {request_data["time"]})\
        |> filter(fn:(r) => r._measurement == "my_measurement")\
        |> filter(fn:(r) => r._field == "{request_data["sensor"]}")'

        data = query_api.query(query=query, org=INFLUXDB_ORG)
        data = loads(data.to_json())

        return Response(data)



class TestApi(APIView):
    """
    this view is just for testing the api
    """
    permission_classes = [IsAdminUser]

    def get(self, request):

        return Response("asqar sagsibil ")