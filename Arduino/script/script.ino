
#include "C:/Users/Arman/Desktop/robotic-project/Arduino/libraries/Lib.h"

// MQTT topics for the first controller
const char gasTopic[] = "controller1/gas";
const char lampTopic[] = "controller1/lamp";
const char waterTopic[] = "controller1/water";
const char lockTopic[] = "controller1/lock";

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
    // subscribe to the topics
    MQTTclient.subscribe(gasTopic);
    MQTTclient.subscribe(lampTopic);
    MQTTclient.subscribe(waterTopic);
    MQTTclient.subscribe(lockTopic);
  }

  
  if (TOPIC == gasTopic)
  {
    /* code */
  }
  else if (TOPIC == lampTopic)
  {
    /* code */
  }
  else if (TOPIC == waterTopic)
  {
    /* code */
  }
  else if (TOPIC == lockTopic)
  {
    /* code */
  }

  MQTTclient.loop();
  delay(10);
  TOPIC = "";
  MSG = "";
}
