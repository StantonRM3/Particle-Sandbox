#include "MQTT.h"

void callback(char *topic, byte *payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * exp) iot.eclipse.org is Eclipse Open MQTT Broker: https://iot.eclipse.org/getting-started
 * MQTT client("iot.eclipse.org", 1883, callback);
 **/
byte server[] = {192, 168, 0, 67};

MQTT client(server, 1883, callback);

#define BUF_SIZE       64
#define JSONSTR_LEN   128

char tmpReceivedMQTTTopic[BUF_SIZE];
char tmpReceivedMQTTData[BUF_SIZE];
bool newMQTTReceived = false;

// recieve message
void callback(char *topic, byte *payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;

  strncpy(tmpReceivedMQTTTopic, topic, BUF_SIZE);
  strcpy(tmpReceivedMQTTData, p);

  newMQTTReceived = true;
}

void setup()
{
  unsigned long timestamp = millis();
  char clientName[32];

  clientName[sprintf(clientName, "argon_%ld", timestamp)] = '\0';

  // connect to the server
  client.connect(clientName);

  // publish/subscribe
  if (client.isConnected())
  {
    Particle.publish("MQTTCON", clientName);
    client.publish("particle/outTopic/message", "hello world");
    client.subscribe("particle/inTopic/message");
  }
  else {
    Particle.publish("MQTTCONFAIL", clientName);
  }
}

void loop()
{
  if (client.isConnected())
    client.loop();

  if (newMQTTReceived == true)
  {
    char jsonString[JSONSTR_LEN];

    sprintf(jsonString, "{ \"topic\":\"%s\",\"data\":\"%s\" }", tmpReceivedMQTTTopic, tmpReceivedMQTTData);

    // client.publish("particle/outTopic/echo", tmpReceivedMQTT);

    // Push the MQTT message out to the other devices and see if they are interested...
    Particle.publish("MQTT-Part", jsonString);
    Mesh.publish("MQTT-Mesh", jsonString);

    newMQTTReceived = false;
  }
}
