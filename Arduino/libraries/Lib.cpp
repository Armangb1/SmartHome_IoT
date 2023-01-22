#include "Lib.h"

String MSG;
String TOPIC;
SoftwareSerial ESPserial(RX_PIN, TX_PIN);
WiFiEspClient ESPclient;
PubSubClient MQTTclient(SERVER_IP, SERVER_PORT, callback, ESPclient);

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