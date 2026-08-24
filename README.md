# SweetSignal — SmartHome IoT

End-to-end smart-home platform: an Arduino-based controller publishes sensor data
over MQTT, Node-RED bridges it into InfluxDB, and a Django REST API serves the
time-series history and relays actuator commands back to the hardware. A Flutter
mobile app provides JWT-authenticated login and device control.

## Architecture

```
                    sensor data (gas / lamp / water / lock)
  ┌──────────────┐   MQTT publish   ┌────────────┐   subscribe   ┌───────────┐
  │   Arduino    │ ───────────────▶ │  Mosquitto │ ────────────▶ │ InfluxDB  │
  │ ESP01 + L298 │                  │   :1883    │  (Node-RED)   │  :8086    │
  │ stepper,     │ ◀─────────────── │            │               └─────┬─────┘
  │ relay, taps) │  actuator cmds   └─────▲──────┘                     │ Flux query
  └──────────────┘                        │ publish                    ▼
                                   ┌──────┴──────────────────────────────────┐
                                   │ Django REST API  :8000                  │
                                   │  GET/POST /api/read/<topic>/            │
                                   │  POST     /api/write/<topic>/           │
                                   │  JWT auth (simplejwt), staff-only       │
                                   └─────────────────────────────────────────┘
```

| Component      | Tech                                            | Location              |
|----------------|--------------------------------------------------|-----------------------|
| Firmware       | C++ (Arduino), PubSubClient, WiFiEsp             | `Arduino/`            |
| Message broker | Eclipse Mosquitto                                | `docker/mosquitto/`   |
| Bridge         | Node-RED flow: MQTT → InfluxDB                   | `docker/node-red/`    |
| Time series DB | InfluxDB 2 (org `sweetsignal`, bucket `IOT-buck`) | `docker/influxdb.env.example` |
| Backend API    | Django 4 + DRF + simplejwt + gunicorn            | `backend/`            |
| Mobile app     | Flutter                                          | `frontend/flutter/ui` |

## Quickstart

Prerequisites: Docker + Docker Compose.

```sh
# 1. Configure secrets (never committed)
cp docker/influxdb.env.example docker/influxdb.env   # set a real password/token
cp backend/.env.example backend/.env                 # match INFLUXDB_* values above

# 2. Create the MQTT password file (gitignored)
cd docker && ./mosquitto/create-passfile.sh admin admin

# 3. Build & run everything
docker compose up -d --build

# 4. One-time: install the InfluxDB palette into the bind-mounted user dir
#    (node_modules is not committed)
cd docker && docker compose exec -T -w /data node-red npm install --omit=dev && \
    docker compose restart node-red
```

| Service   | URL                     |
|-----------|-------------------------|
| Django API | http://localhost:8000 |
| Node-RED   | http://localhost:1880 |
| InfluxDB UI| http://localhost:8086 |
| MQTT       | localhost:1883        |

Create an API user (all `/api/*` endpoints require a **staff** account):

```sh
docker compose exec backend python manage.py createsuperuser
```

> Running the backend outside Compose? `cp backend/.env.example backend/.env`,
> point `MQTT_HOST=localhost`, then `python manage.py migrate && runserver`.

## API usage

Obtain a JWT pair, then call the read/write endpoints:

```sh
TOKEN=$(curl -s -X POST http://localhost:8000/api/auth/token/ \
  -d "username=admin&password=..." | jq -r .access)

# Last 30 minutes of gas-sensor readings (from InfluxDB)
curl -H "Authorization: Bearer $TOKEN" http://localhost:8000/api/read/gas/

# Custom time window
curl -X POST -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"time": "-2h"}' http://localhost:8000/api/read/gas/

# Toggle the lamp (published to MQTT topic controller1/lamp)
curl -X POST -H "Authorization: Bearer $TOKEN" \
  -d "value=1" http://localhost:8000/api/write/lamp/
```

## MQTT topics

Documented in `README.md` historically; convention:

| Topic                | Direction          | Payload        |
|----------------------|--------------------|----------------|
| `controller1/gas`    | sensor → broker    | ppm reading    |
| `controller1/lamp`   | broker → actuator  | `1` / `0`      |
| `controller1/water`  | broker → actuator  | `1` / `0`      |
| `controller1/lock`   | broker → actuator  | `1` / `0`      |

The write endpoint hardcodes the `controller1/` prefix (`api/views.py`).

## Hardware (`Arduino/script/`)

ESP8266 (ESP-01) on SoftwareSerial pins 2/3 drives four subsystems — pin map in
`Lib.h`:

| Subsystem | Hardware                          | Pins                       |
|-----------|-----------------------------------|----------------------------|
| Gas valve | L298N + stepper (1385° rotation)  | 4–7                        |
| Lamp      | Relay/digital out                 | 9                          |
| Lock      | Digital out                       | 8                          |
| Water tap | Motor + open/closed limit switches| 10, 11 (+12/13 feedback)   |

Copy `Arduino/libraries` content or the sketch-local `Lib.h` into your Arduino
environment; fill in Wi-Fi/MQTT credentials marked `YOUR_*`. The
`wifi_broker_connection` sketch is a minimal connectivity smoke test.

## Development

```sh
cd backend
python manage.py test        # unit tests (InfluxDB/MQTT are mocked)
pycodestyle .                # style check (config in setup.cfg)
```

CI runs style checks and tests on every push (`.github/workflows/ci.yml`).
