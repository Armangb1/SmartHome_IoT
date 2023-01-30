from django.shortcuts import render

# Create your views here.
from rest_framework.views import APIView
from rest_framework.permissions import IsAdminUser
from rest_framework.response import Response
from config.settings import (INFLUXDB_BUCKET,
                             INFLUXDB_TOKEN,
                             INFLUXDB_ORG,
                             INFLUXDB_URL,
                             MQTT_PORT,
                             MQTT_HOST,
                             MQTT_USER,
                             MQTT_PASS)
from influxdb_client import InfluxDBClient
from json import loads
import paho.mqtt.client as mqtt


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

    def get(self, request, topic):

        client = InfluxDBClient(url=INFLUXDB_URL,
                                token=INFLUXDB_TOKEN, org=INFLUXDB_ORG)

        query_api = client.query_api()

        #TODO : make this ok
        query = f'from(bucket:"{INFLUXDB_BUCKET}")\
        |> range(start: -30m)\
        |> filter(fn:(r) => r._measurement == "{topic}")\
        |> filter(fn:(r) => r._field == "value")'

        data = query_api.query(query=query, org=INFLUXDB_ORG)
        data = loads(data.to_json())

        return Response(data)

    def post(self, request, topic):

        request_data = request.data

        client = InfluxDBClient(url=INFLUXDB_URL,
                                token=INFLUXDB_TOKEN, org=INFLUXDB_ORG)
        query_api = client.query_api()

        # TODO: add some more featue to the query : measurement
        query = f'from(bucket:"{INFLUXDB_BUCKET}")\
        |> range(start: {request_data["time"]})\
        |> filter(fn:(r) => r._measurement == {topic})\
        |> filter(fn:(r) => r._field == "value")'

        data = query_api.query(query=query, org=INFLUXDB_ORG)
        data = loads(data.to_json())

        return Response(data)


class WriteActuator(APIView):
    """
    this view post data to the actuators

"""
    permission_classes = [IsAdminUser]

    def post(self, request, topic):

        mqtt_client = mqtt.Client("python-mqtt-client")
        mqtt_client.username_pw_set(MQTT_USER, MQTT_PASS)
        mqtt_client.connect(MQTT_HOST, MQTT_PORT, 60)

        #TODO: make it general for all controller
        result = mqtt_client.publish(
            f"controller1/{topic}", request.data["value"])

        if result[0] == 0:
            return Response({"status": "success"})
        else:
            return Response({"status": "failed"})
