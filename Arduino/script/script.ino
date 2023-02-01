
// TODO: check if its work with esp8266
#include "Lib.h"

// topics

long lastReconnectAttempt = 0;

void setup()
{
	// initialize serial for debugging
	Serial.begin(115200);

	pinMode(LAMP_PIN, OUTPUT);
	pinMode(LOCK_PIN, OUTPUT);
	pinMode(WATER_PIN_1, OUTPUT);
	pinMode(WATER_PIN_2, OUTPUT);
	pinMode(WATER_OPENED_PIN, INPUT);
	pinMode(WATER_CLOSED_PIN, INPUT);

	wifi_connect();

	gasValveStepper.setSpeed(STEPPER_SPEED);
	lastReconnectAttempt = 0;
}

void loop()
{
	// old code

	// if (!MQTTclient.connected())
	// {
	// 	reconnect();
	// 	// you can publish here for debug
	// 	MQTTclient.publish("topic", "hello from controller");

	// 	MQTTclient.subscribe(gasTopic);
	// 	MQTTclient.subscribe(lampTopic);
	// 	MQTTclient.subscribe(waterTopic);
	// 	MQTTclient.subscribe(lockTopic);
	// 	MQTTclient.subscribe(gasTopic);
	// }

	// MQTTclient.loop();
	// delay(200);
	// new code
	if (!MQTTclient.connected())
	{
		long now = millis();
		if (now - lastReconnectAttempt > 5000)
		{
			lastReconnectAttempt = now;
			// Attempt to reconnect
			if (reconnect())
			{
				lastReconnectAttempt = 0;
				Serial.println("connected");
			}
		}
	}
	else
	{
		// Client connected
		// long t_0 = millis();
		// long t_1 = 0;
		// while ((t_1 - t_0 < 300))
		// {
		// 	t_1 = millis();
		// }
		MQTTclient.loop();
		delay(400);

		if (TOPIC == gasTopic)
		{
			if (MSG == "1") // open the valve
			{
				gasValueControl(true);
			}
			else if (MSG == "0") // close the valve
			{
				gasValueControl(false);
			}
		}
		else if (TOPIC == lampTopic)
		{
			if (MSG == "1")
			{
				digitalWrite(LAMP_PIN, HIGH);
			}
			else if (MSG == "0")
			{
				digitalWrite(LAMP_PIN, LOW);
			}
		}
		else if (TOPIC == waterTopic)
		{
			if (MSG == "1")
			{
				waterTapControl(true);
			}
			else if (MSG == "0")
			{
				waterTapControl(false);
			}
		}
		else if (TOPIC == lockTopic)
		{
			if (MSG == "1")
			{

				digitalWrite(LOCK_PIN, HIGH);
			}
			else if (MSG == "0")
			{

				digitalWrite(LOCK_PIN, LOW);
			}
		}
	}
	MSG = "";
	TOPIC = "";
}

// void reconnect()
// {
// 	// Loop until we're reconnected
// 	while (!MQTTclient.connected())
// 	{
// 		Serial.print("Attempting MQTT connection...");
// 		// Attempt to connect
// 		if (MQTTclient.connect("arduinoClient", MQTT_USER, MQTT_PASS, gasTopic, 1, true, "0")) // TODO: change the last parameter to 0
// 		{
// 			Serial.println("connected");
// 		}
// 		else
// 		{
// 			Serial.print("failed, rc=");
// 			Serial.print(MQTTclient.state());
// 			Serial.println(" try again in 5 seconds");
// 			// Wait 5 seconds before retrying
// 			delay(5000);
// 		}
// 	}
// }
