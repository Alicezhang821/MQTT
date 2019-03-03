#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <dht11.h>


dht11 DHT;
#define DHT11_PIN 9
int tempF;
int tempC;
int humidity;
 

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(10, 1, 1, 17);
IPAddress server(10, 1, 1, 9);
 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
 
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
 
void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("arduinoClient")) {
      Serial.println("connected");
      mqttClient.subscribe("inTopic");
     
    } else {
      Serial.print("failed, rc=");
     // Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
  Serial.begin(57600);
 
  mqttClient.setServer(server, 1883);
  mqttClient.setCallback(callback);
 
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

 String buildJson() {
  tempC = DHT.temperature;
  humidity =DHT. humidity;
  String data = "{";
  data+="\n";
  data+="\"myName\": \"Arduino DHT11\",";
  data+="\n";
  data+="\"temperature (C)\": ";
  data+=(int)tempC;
  data+= ",";
  data+="\n";
  data+="\"humidity\": ";
  data+=(int)humidity;
  data+="\n";
  data+="}";
  return data;
}
void loop()
{
  if (!mqttClient.connected()) {
    reconnect();
  }
   
if (mqttClient.connected()){
  int chk;
  chk = DHT.read(DHT11_PIN); 
  Serial.print(DHT.humidity,1);
  Serial.print(",\t");
  Serial.println(DHT.temperature,1);
 
    String json = buildJson();
    char jsonStr[200];
    json.toCharArray(jsonStr,200);
    
  mqttClient.publish("inTopic",jsonStr);
  
  }

  delay(1000);
  mqttClient.loop();
}

