/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define PRESMSGINTVL 30000

const int buzzer = D5; //Buzzer control port, default D5
int freq[]={1047,1175,1319,1397,1568,1760,1976,2093};//Note name: C6 D6 E6 F6 G6 A6 B6 C7   http://newt.phys.unsw.edu.au/jw/notes.html
String note[]={"C6", "D6", "E6", "F6", "G6", "A6", "B6", "C7"};

const char* ssid     = "comPVter";
const char* password = ""; //add wifi password here!

const char* mqtt_server = ""; //Add mqtt broker here

WiFiClient espClient;
PubSubClient client(espClient);
long lastPresMsg = 0;
char msg[60];
int value = 0;

const char* nodeId = "CPVWM##"; // ## unique serial number of the node! 01,02...

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED,HIGH);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
  Serial.begin(115200);
  setup_wifi();
  
  pinMode(buzzer, OUTPUT);
  
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
  
  String trigger = root["command"];   Serial.print("trigger: ");    Serial.println(trigger);
  if (trigger == "longPress")
  {
    Serial.println("long press");
    for(int i=0; i<8; i++){
     analogWriteRange(freq[i]);
     Serial.print("Note name: ");
     Serial.print(note[i]);
     Serial.print(", Freq: ");
     Serial.print(freq[i]);
     Serial.println("Hz");
     
     analogWrite(buzzer, 512);
     delay(1000);
     analogWrite(buzzer, 0);
     pinMode(buzzer, OUTPUT);
     digitalWrite(buzzer, LOW);
     delay(1000);
     }
   Serial.println("STOP");
   delay(5000);
  }
    if (trigger == "doubleClick")
  {
    Serial.println("double click");
  }
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
      client.subscribe("compvter/CPVWM##/actions");
      client.subscribe("compvter/actions");
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
