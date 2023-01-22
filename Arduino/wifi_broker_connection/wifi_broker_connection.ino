#include <SoftwareSerial.h>

#include <WiFiEsp.h>
#include <WiFiEspClient.h>

#include <PubSubClient.h>

#define RX_PIN 2
#define TX_PIN 3
#define SSID_NAME "YOUR_WIFI_SSID" // put your SSID here
#define WIFI_PASS "***REMOVED***"                 // put your WIFI password here
#define MQTT_USER "controller1"              // put your MQTT username here
#define MQTT_PASS "***REMOVED***"                 // put your MQTT password here
#define SERVER_IP "192.168.1.13"             // put your server IP here
#define SERVER_PORT 1883                     // put your server port here

void reconnect();
void callback(char *topic, byte *payload, unsigned int length);
void wifi_connect();

SoftwareSerial ESPserial(RX_PIN, TX_PIN);
WiFiEspClient ESPclient;

PubSubClient MQTTclient(SERVER_IP, SERVER_PORT, callback, ESPclient);

String MSG;
String TOPIC;

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);

  wifi_connect();
}

void loop()
{
  if (!MQTTclient.connected())
  {
    reconnect();
    // you can publish here for debug
    MQTTclient.publish("topic", "hello from controller");
    // subscribe here
    MQTTclient.subscribe("topic");
    //------------------------
  }
  MQTTclient.loop();
  delay(10);
}

void reconnect()
{
  // Loop until we're reconnected
  while (!MQTTclient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (MQTTclient.connect("arduinoClient", MQTT_USER, MQTT_PASS))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(MQTTclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

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