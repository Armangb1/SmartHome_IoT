#ifndef LIB_H
#define LIB_H

#include <Arduino.h>

// library
#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <PubSubClient.h>

// config
#define RX_PIN 2
#define TX_PIN 3
#define SSID_NAME "YOUR_WIFI_SSID" // put your SSID here
#define WIFI_PASS "***REMOVED***"                 // put your WIFI password here
#define MQTT_USER "controller1"              // put your MQTT username here
#define MQTT_PASS "***REMOVED***"                 // put your MQTT password here
#define SERVER_IP "192.168.1.13"             // put your server IP here
#define SERVER_PORT 1883                     // put your server port here

// global variables

extern String MSG;
extern String TOPIC;

// function
void reconnect();
void callback(char *topic, byte *payload, unsigned int length);
void wifi_connect();

// objects

extern WiFiEspClient ESPclient;
extern PubSubClient MQTTclient;
extern SoftwareSerial ESPserial;

#endif