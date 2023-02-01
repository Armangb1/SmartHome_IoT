// MQTT connection with ESP01
#ifndef Lib_H
#define Lib_H

// MQTT & WIFI details:
// MQTT library:
#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <PubSubClient.h>

// MQTT config
#define RX_PIN 2
#define TX_PIN 3
#define SSID_NAME "YOUR_WIFI_SSID" // put your SSID here
#define WIFI_PASS "***REMOVED***"                 // put your WIFI password here
#define MQTT_USER "controller1"              // put your MQTT username here
#define MQTT_PASS "***REMOVED***"                 // put your MQTT password here
#define SERVER_IP "192.168.1.13"             // put your server IP here
#define SERVER_PORT 1883                     // put your server port here

// MQTT functions
bool reconnect();
void callback(char *topic, byte *payload, unsigned int length);
void wifi_connect();

// MQTT objects
extern SoftwareSerial ESPserial;
extern WiFiEspClient ESPclient;
extern PubSubClient MQTTclient;

// MQTT global variables
extern String MSG;
extern String TOPIC;

// MQTT topics
extern const char gasTopic[];
extern const char lampTopic[];
extern const char waterTopic[];
extern const char lockTopic[];

#include <Stepper.h>

// gas valve config
#define L298_IN1 7
#define L298_IN2 6
#define L298_IN3 5
#define L298_IN4 4
#define REVERSE_INPUT_GAS true

#define VALVE_TURN 1385     // 1380 ii degree
#define STEPPER_SPEED 50    // 50 rpm
#define STEP_PER_DEGREE 1.8 // 1.8 step per degree

#define LAMP_PIN 9
// gas valve objects
extern Stepper gasValveStepper;

// gas valve functions
bool gasValueControl(bool InputCommand);

// lamp details:
// lamp config
// #define LAMP_PIN 9

// lock details:
// lock config
#define LOCK_PIN 8

// water details:
// water config
#define WATER_PIN_1 10
#define WATER_PIN_2 11
#define WATER_OPENED_PIN 13
#define WATER_CLOSED_PIN 12
#define REVERSE_INPUT_WATER false

bool waterTapControl(bool InputCommand);


#endif