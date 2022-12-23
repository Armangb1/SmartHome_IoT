#include <SoftwareSerial.h>

#include <WiFiEsp.h>
#include <WiFiEspClient.h>

#include <PubSubClient.h>

#define RX_PIN 2
#define TX_PIN 3
#define SSID_NAME "Xiaomi_0396" // put your SSID here
#define WIFI_PASS "***REMOVED***"    // put your WIFI password here
#define MQTT_USER "controller1" // put your MQTT username here
#define MQTT_PASS "ABCDEFG"     // put your MQTT password here

SoftwareSerial ESPserial(RX_PIN, TX_PIN);
int status = WL_IDLE_STATUS;

WiFiEspClient ESPclient;
IPAddress serverIP(192, 168, 31, 77); // server IP Address
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
PubSubClient MQTTclient(serverIP, 1883, callback, ESPclient);

String MSG = "";
String TOPIC = "";

void setup()
{
    // initialize serial for debugging
    Serial.begin(115200);
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

    // connect to MQTT broker
    while (!MQTTclient.connected())
    {
        Serial.println("connection to server failed...");
        Serial.println("attemping to reconnect...");
        MQTTclient.connect("controller1", MQTT_USER, MQTT_PASS);
        Serial.println(MQTTclient.state());
    }
    Serial.println("connected to the server");
    MQTTclient.publish("topicout", "controller1 vasl shod");
    MQTTclient.subscribe("topicin");
}

void loop()
{
    // enter your codde here

    MQTTclient.loop();
}

// void callback(char *topic, byte *payload, unsigned int length)
// {
//     Serial.print("Message arrived [");
//     Serial.print(topic);
//     TOPIC = topic;
//     Serial.print("] ");
//     for (int i = 0; i < length; i++)
//     {
//         Serial.print((char)payload[i]);
//         MSG[i] = (char)payload[i];
//     }
//     Serial.println();
// }