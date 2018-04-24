#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <ArduinoJson.h>

//for LED status
#include <Ticker.h>
Ticker ticker;

const char* ssid     = "comPVter";
const char* password = "PVassword";

const char* serverurl = "samuele.ddns.net";
//IPAddress mqtt_server_ip(52, 209, 215, 251);

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long lastPresMsg = 0;
char msg[90];

const char* nodeId = "CPV01";

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void setup() {

  Serial.begin(115200);

  setup_wifi();

  pinMode(D1, OUTPUT); digitalWrite(D1, HIGH);
  pinMode(D2, OUTPUT); digitalWrite(D2, HIGH);
  pinMode(D3, OUTPUT); digitalWrite(D3, HIGH);
  pinMode(D4, OUTPUT); digitalWrite(D4, HIGH);

  client.setServer(serverurl, 1883);
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
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(message);

  String outid = root["id"];        Serial.print("id: "); Serial.println(outid.toInt());
  String command = root["command"]; Serial.print("command: "); Serial.println(command.toInt());

  String R = root["color"][0]; Serial.print("R: "); Serial.println(R.toInt());
  String G = root["color"][1]; Serial.print("G: "); Serial.println(G.toInt());
  String B = root["color"][2]; Serial.print("B: "); Serial.println(B.toInt());


  if ( (outid.toInt() == 1) && (command.toInt() == 0) )
    digitalWrite(D1, HIGH);
  if ( (outid.toInt() == 1) && (command.toInt() == 1) )
    digitalWrite(D1, LOW);

  if ( (outid.toInt() == 2) && (command.toInt() == 0) )
    digitalWrite(D2, HIGH);
  if ( (outid.toInt() == 2) && (command.toInt() == 1) )
    digitalWrite(D2, LOW);

  if ( (outid.toInt() == 3) && (command.toInt() == 0) )
    digitalWrite(D3, HIGH);
  if ( (outid.toInt() == 3) && (command.toInt() == 1) )
    digitalWrite(D3, LOW);

  if ( (outid.toInt() == 4) && (command.toInt() == 0) )
    digitalWrite(D4, HIGH);
  if ( (outid.toInt() == 4) && (command.toInt() == 1) )
    digitalWrite(D4, LOW);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    sprintf(msg, nodeId);
    sprintf(&msg[3], " is dead");
    if (client.connect(nodeId, "CompPVter/LWT", MQTTQOS1, false, msg)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("compPVter/presentations", nodeId);
      // ... and resubscribe
      client.subscribe("compPVter/commands");
      client.subscribe("compPVter/colors");
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

}
