/*

*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Switch.h"

#define MSGINTVL 10000
#define PRESMSGINTVL 30000

#define buttonPin D3
Switch button = Switch(buttonPin);  // Switch between a digital pin and GND

const char* ssid     = "comPVter";
const char* password = ""; //add wifi password here!

const char* mqtt_server = ""; //Add mqtt broker here

WiFiClient espClient;
PubSubClient client(espClient);

long lastPresMsg = 0;
char msg[60];

const char* nodeId = "CPVWM##"; // ## unique serial number of the node! 01,02...

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED,HIGH);
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, 1883);

}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while ((WiFi.status() != WL_CONNECTED)) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
    sprintf(msg, nodeId);
    sprintf(&msg[7], " is dead");
    if (client.connect(nodeId, "LWT", MQTTQOS2, false, msg)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Presentations", nodeId);
      // ... and resubscribe
      digitalWrite(BUILTIN_LED,LOW);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() 
{
  button.poll();

  if (!client.connected()) {
    digitalWrite(BUILTIN_LED,HIGH);
    reconnect();
  }
  client.loop();

  if (button.doubleClick()) {
    Serial.print("dbutton.doubleClick ");
    sprintf(msg, "{\"NID\": \""); //Serial.print("strlen: "); Serial.println(strlen(msg)); Serial.print("sizeof nodeId: "); Serial.println(sizeof(nodeId));
    sprintf(&msg[strlen(msg)], nodeId);
    sprintf(&msg[strlen(msg)], "\", \"command\": \"doubleClick\"}");

    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("compvter/CPVWM02/triggers", msg);
  }

  if (button.longPress()) {
    Serial.print("longPress ");
    sprintf(msg, "{\"NID\": \""); //Serial.print("strlen: "); Serial.println(strlen(msg)); Serial.print("sizeof nodeId: "); Serial.println(sizeof(nodeId));
    sprintf(&msg[strlen(msg)], nodeId);
    sprintf(&msg[strlen(msg)], "\", \"command\": \"longPress\"}");

    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("compvter/CPVWM02/triggers", msg);
  }

long  now = millis();
  if ( (now - lastPresMsg) > PRESMSGINTVL) 
  {
    lastPresMsg = now;
    client.publish("Presentations", nodeId);
  }
}
