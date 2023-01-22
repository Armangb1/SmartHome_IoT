
#include "C:/Users/Arman/Desktop/robotic-project/Arduino/libraries/Lib.h"

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
  }

  MQTTclient.loop();
  delay(10);
}
