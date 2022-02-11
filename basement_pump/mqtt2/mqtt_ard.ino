/*
Arduino: MQTT to Serial Bridge Using ESP32
Sankar Cheppali | December 9, 2017 | DIY electronics, ESP32 | 1 Comment
LinkedInTwitterWhatsAppFacebookShare
Most of the applications involved with ESP32 requires some sort of
data transport, means the device should be able to send data over network to
some other device/system.
There are several methods available to do that.
You can find brief overview of the available protocols here .
In this blog we will see who to connect ESP32 to a MQTT broker using pubsub
client.

The system we are designing in this blog will be capable of sending the text
received on serial line to MQTT broker.
It will also subscribe to a specified channel and send any message
received on channel to the serial line.
It works as a serial to MQTT bridge.

ESP32 based Serial to MQTT bridge
ESP32 based Serial to MQTT bridge
Environment requirements:
you need to have a ESP32
setup your Arduino IDE to program ESP32
PubSubClient arduino library
(if you haven’t already installed it ,
you can install it from Sketch->Include library->Manage Libraries)
mqtt broker , if you don’t have one you can use eclipse
Paho broker for experimentation (host: iot.eclipse.org , port : 1883).
 

ESP32 MQTT Code:
On setup function we will try to connect to access point.
Once we are connected to access point we will connect to given MQTT broker
and subscribe to “/icircuit/ESP32/serialdata/rx” channel.
Messages published to this channel will be printed on our
serial console.
All the messages sent from serial console will be published to
“/icircuit/ESP32/serialdata/tx’.

Upload the following code to ESP32.
Don’t forget to change the network and MQTT broker details before uploading.
if you are using eclipse mqtt broker, no need of any username/password.
*/
#include <WiFi.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.
const char* ssid = "SSID";
const char* password = "pwd";
const char* mqtt_server = "iot.eclipse.org";
#define mqtt_port 1883
#define MQTT_USER "username"
#define MQTT_PASSWORD "password"
#define MQTT_SERIAL_PUBLISH_CH "/icircuit/ESP32/serialdata/tx"
#define MQTT_SERIAL_RECEIVER_CH "/icircuit/ESP32/serialdata/rx"

WiFiClient wifiClient;

PubSubClient client(wifiClient);

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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("/icircuit/presence/ESP32/", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}


void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}
void loop() {
   client.loop();
   if (Serial.available() > 0) {
     char bfr[501];
     memset(bfr,0, 501);
     Serial.readBytesUntil( '\n',bfr,500);
     publishSerialData(bfr);
   }
 }