/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN D4     // what pin we're connected to
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define MSGINTVL 10000
#define PRESMSGINTVL 30000

const char* ssid     = "comPVter";
const char* password = ""; //add wifi password here!

const char* mqtt_server = ""; //Add mqtt broker here

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
long lastPresMsg = 0;
char msg[60];
int value = 0;

const char* nodeId = "CPVWM##"; // ## unique serial number of the node! 01,02...

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED,HIGH);
  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //boolean connect(const char* id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
    sprintf(msg, nodeId);
    sprintf(&msg[7], " is dead");
    if (client.connect(nodeId, "compvter/LWT", MQTTQOS2, false, msg)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("compvter/presentations", nodeId);
      // ... and resubscribe
      //client.subscribe("Metrics");
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
  if (!client.connected()) {
    digitalWrite(BUILTIN_LED,HIGH);
    reconnect();
  }
  client.loop();

  long now = millis();
  if ((now - lastMsg > MSGINTVL)) {
    lastMsg = now;
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    float hi = dht.computeHeatIndex(t, h, false);

    int hint = 1000 * h;
    int tint = 1000 * t;
    int hiint = 1000 * hi;

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) ) {
      Serial.println("Failed to read from DHT sensor!");
      client.publish("Errors", "Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("Heat index: ");
    Serial.print(hi);
    Serial.println(" *C");

    sprintf(msg, "{\"NID\": \"CPVWM01\", \"sensor\": \"DHT11\", \"T\":%d, \"RH\":%d}", (int)t, (int)h);

    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("compvter/CPVWM01/metrics", msg);
  }

  now = millis();
  if ( (now - lastPresMsg) > PRESMSGINTVL)
  {
    lastPresMsg = now;
    client.publish("compvter/presentations", nodeId);
  }
}
