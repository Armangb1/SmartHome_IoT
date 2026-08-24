from unittest import mock

from django.contrib.auth.models import User
from rest_framework.test import APIClient
from rest_framework_simplejwt.tokens import RefreshToken

from django.test import TestCase

USERNAME = "tester"
PASSWORD = "secret12345"
AUTH_URL = "/api/auth/token/"


def create_user(**kwargs):
    defaults = {"username": USERNAME, "password": PASSWORD}
    defaults.update(kwargs)
    return User.objects.create_user(**defaults)


class AuthTests(TestCase):
    """JWT token endpoint issues access tokens for valid credentials."""

    def setUp(self):
        self.client = APIClient()
        self.user = create_user()

    def test_obtain_token(self):
        response = self.client.post(AUTH_URL, {"username": USERNAME, "password": PASSWORD})
        self.assertEqual(response.status_code, 200)
        self.assertIn("access", response.data)

    def test_reject_bad_credentials(self):
        response = self.client.post(AUTH_URL, {"username": USERNAME, "password": "wrong"})
        self.assertEqual(response.status_code, 401)


class ApiPermissionTests(TestCase):
    """All /api/* views require an authenticated staff user."""

    def setUp(self):
        create_user()
        self.staff = APIClient()
        staff = create_user(username="staff", is_staff=True)
        self.staff.credentials(
            HTTP_AUTHORIZATION=f"Bearer {RefreshToken.for_user(staff).access_token}"
        )

    def test_read_requires_authentication(self):
        response = APIClient().get("/api/read/gas/")
        self.assertEqual(response.status_code, 401)

    def test_write_requires_staff(self):
        plain = APIClient()
        plain_user = User.objects.get(username='tester')
        plain.credentials(
            HTTP_AUTHORIZATION=f"Bearer {RefreshToken.for_user(plain_user).access_token}"
        )
        response = plain.post("/api/write/lamp/", {"value": "1"})
        self.assertEqual(response.status_code, 403)


class QuerySensorsTests(TestCase):
    def setUp(self):
        self.client = APIClient()
        staff = create_user(is_staff=True)
        self.client.credentials(
            HTTP_AUTHORIZATION=f"Bearer {RefreshToken.for_user(staff).access_token}"
        )

    @mock.patch("api.views.InfluxDBClient")
    def test_get_queries_influxdb_for_topic(self, influx_client_cls):
        instance = influx_client_cls.return_value
        query_api = instance.query_api.return_value
        query_api.query.return_value.to_json.return_value = '[{"_value": 25.3}]'

        response = self.client.get("/api/read/gas/")

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.data, [{"_value": 25.3}])
        query = query_api.query.call_args.kwargs["query"]
        self.assertIn('r._measurement == "gas"', query)
        instance.close.assert_called_once()


class WriteActuatorTests(TestCase):
    def setUp(self):
        self.client = APIClient()
        staff = create_user(is_staff=True)
        self.client.credentials(
            HTTP_AUTHORIZATION=f"Bearer {RefreshToken.for_user(staff).access_token}"
        )

    @mock.patch("api.views.mqtt")
    def test_post_publishes_to_controller_topic(self, mqtt_module):
        mqtt_module.Client.return_value.publish.return_value = (0, 1)

        response = self.client.post("/api/write/lamp/", {"value": "1"})

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.data["status"], "success")
        mqtt_module.Client.return_value.publish.assert_called_once_with("controller1/lamp", "1")

    @mock.patch("api.views.mqtt")
    def test_post_reports_failure_when_broker_unreachable(self, mqtt_module):
        mqtt_module.Client.return_value.connect.side_effect = OSError("broker down")

        response = self.client.post("/api/write/lamp/", {"value": "1"})

        self.assertEqual(response.data["status"], "failed")
