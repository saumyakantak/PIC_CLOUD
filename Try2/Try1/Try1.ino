#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Update these with values suitable for your network.

const char* ssid = "JioFi3_4D6DFA";
const char* password = "1t3xr46d5p";
const char* mqtt_server = "3.229.243.137";

//StaticJsonDocument<200> arduino_packet;
void serialEvent();
String inputString = "";         // a String to hold incoming data
bool stringComplete = false; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  /*Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  */
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == 'Y') 
  {
    Serial.write('Y');
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("20208888/cube/data", "hello world");
      // ... and resubscribe
      client.subscribe("20208888/cube/sub");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() 
{

  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  
  serialEvent();

  StaticJsonDocument<256>doc;
  char output[256];
  doc["msgtype"]="data";
  doc["datatype"]="temperature";
  doc["data"]=inputString.toInt();
  serializeJson(doc,output);
  if (stringComplete) 
  {
      client.publish("20208888/cube/data",output);
    //Serial.println(inputString);
    inputString = "";
    stringComplete = false;
  }  
}

void serialEvent() 
{
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if(inChar!='\n')
      inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n')
    {
      stringComplete = true;
      //Serial.write("Y");
    }
  }
}
