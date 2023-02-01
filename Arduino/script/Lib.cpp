#include "Lib.h"

SoftwareSerial ESPserial(RX_PIN, TX_PIN);
WiFiEspClient ESPclient;
PubSubClient MQTTclient(SERVER_IP, SERVER_PORT, callback, ESPclient);

String MSG = "";
String TOPIC = "";

const char gasTopic[] = "controller1/gas";
const char lampTopic[] = "controller1/lamp";
const char waterTopic[] = "controller1/water";
const char lockTopic[] = "controller1/lock";

void wifi_connect()
{
    int status = WL_IDLE_STATUS;
    // initialize serial for ESP module
    ESPserial.begin(9600);
    // initialize ESP module
    WiFi.init(&ESPserial);

    // attempt to connect to WiFi network
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(SSID_NAME);
        // Connect to WPA/WPA2 network
        status = WiFi.begin(SSID_NAME, WIFI_PASS);
    }
    Serial.println("You're connected to the network");
}

void callback(char *topic, byte *payload, unsigned int length)
{
    MSG = "";
    TOPIC = topic;
    for (int i = 0; i < length; i++)
    {
        MSG += (char)payload[i];
    }
    Serial.println(TOPIC + " : " + MSG);
}

boolean reconnect()
{

    if (MQTTclient.connect("arduinoClient", MQTT_USER, MQTT_PASS))
    {
        // Once connected, publish an announcement...
        MQTTclient.publish("outTopic", "hello world");
        // ... and resubscribe
        MQTTclient.subscribe(gasTopic);
        MQTTclient.subscribe(waterTopic);
        MQTTclient.subscribe(lampTopic);
        MQTTclient.subscribe(lockTopic);
    }
    return MQTTclient.connected();
}


Stepper gasValveStepper(360 / STEP_PER_DEGREE, L298_IN1, L298_IN2, L298_IN3, L298_IN4);

bool gasValueControl(bool InputCommand)
{

  if (REVERSE_INPUT_GAS)
  {
    InputCommand = !InputCommand;
  }
  switch (InputCommand)
  {
  case 0:
    gasValveStepper.step(VALVE_TURN / STEP_PER_DEGREE);

    Serial.println("InputCommand == 0, Closing the valve");

    return (true);
    break;
  case 1:
    gasValveStepper.step(-VALVE_TURN / STEP_PER_DEGREE);

    Serial.println("InputCommand == 1, Openning the valve");

    return (true);
    break;
  default:

    Serial.println("InputCommand != 1 || InputCommand != 0, Bad Input");

    return (false);
  }
}

bool waterTapControl(bool InputCommand)
{

  if (REVERSE_INPUT_WATER)
  {
    InputCommand = !InputCommand;
  }
  switch (InputCommand)
  {
  case 0:

    // ============================================
    // for test
    // digitalWrite(WATER_PIN_1, LOW);
    // digitalWrite(WATER_PIN_2, LOW);
    // =================================================

    digitalWrite(WATER_PIN_1, HIGH);
    digitalWrite(WATER_PIN_2, LOW);
    delay(800);
    while (digitalRead(WATER_CLOSED_PIN) == LOW)
    {
      delay(10);

      // digitalWrite(WATER_PIN_1, LOW);
      // delay(90);
      // digitalWrite(WATER_PIN_1, HIGH);
    }

    digitalWrite(WATER_PIN_1, LOW);
    digitalWrite(WATER_PIN_2, LOW);
    Serial.println("InputCommand == 0, Closing the valve");

    return (true);
    break;
  case 1:

    // ============================================
    // for test
    // digitalWrite(WATER_PIN_1, LOW);
    // digitalWrite(WATER_PIN_2, LOW);
    // =================================================

    digitalWrite(WATER_PIN_1, LOW);
    digitalWrite(WATER_PIN_2, HIGH);

    while (digitalRead(WATER_OPENED_PIN) == LOW)
    {
      delay(10);
    }

    digitalWrite(WATER_PIN_1, LOW);
    digitalWrite(WATER_PIN_2, LOW);
    Serial.println("InputCommand == 1, Openning the valve");

    return (true);
    break;
  default:

    Serial.println("InputCommand != 1 || InputCommand != 0, Bad Input");

    return (false);
  }
}

bool lighLamp(bool inputCommand)
{
  int lampVolt = 3;
  int lampPWM = map(lampVolt, 0, 12, 0, 1023);
  Serial.println(lampPWM);
  switch (inputCommand)
  {
  case 0:
    digitalWrite(LAMP_PIN, LOW);
    Serial.println("InputCommand == 0, Turning off the lamp");

    return (true);
    break;
  case 1:

    analogWrite(LAMP_PIN, lampPWM);
    Serial.println("InputCommand == 1, Turning on the lamp");

    return (true);
    break;
  default:

    Serial.println("InputCommand != 1 || InputCommand != 0, Bad Input");

    return (false);
  }
}

