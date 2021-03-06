/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#define PRESMSGINTVL 30000

#define PIN D2

const char* ssid     = "Kalpanet";
const char* password = "kalpak4194"; //add wifi password here!

const char* mqtt_server = "samuele.ddns.net"; //Add mqtt broker here

WiFiClient espClient;
PubSubClient client(espClient);

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
long lastPresMsg = 0;
char msg[60];
int value = 0;

const char* nodeId = "CPVWM##"; // ## unique serial number of the node! 01,02...

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED,HIGH);
  Serial.begin(115200);
  setup_wifi();
  

  pixels.begin();
  
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
  String  message = "";

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print(message);
  Serial.println();

  StaticJsonBuffer<100> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);
  
  String r = root["r"];   Serial.print("r: ");    Serial.println(r);
  String g = root["g"];   Serial.print("g: ");    Serial.println(g);
  String b = root["b"];   Serial.print("b: ");    Serial.println(b);

    pixels.setPixelColor(0, pixels.Color(r.toInt(), g.toInt(), b.toInt())); 
    pixels.show(); // This sends the updated pixel color to the hardware
  
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
      client.subscribe("compvter/web/rgb");
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
    reconnect();
  }
  client.loop();

  long int now = millis();
  if ( (now - lastPresMsg) > PRESMSGINTVL)
  {
    lastPresMsg = now;
    client.publish("compvter/presentations", nodeId);
  }
}
