/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.
*/

#include "EspMQTTClient.h"
#define DAC1 25
#define DAC2 26

EspMQTTClient client(
  "Butter2.4",
  "Butt3r12",
  "192.168.1.50",  // MQTT Broker server ip
  "",//"MQTTUsername",   // Can be omitted if not needed
  "",//"MQTTPassword",   // Can be omitted if not needed
  "TestClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  Serial.begin(115200);
  Serial.println("SimpleMQTTClient WeMOS WiFI etc");
  // Optionnal functionnalities of EspMQTTClient : 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}



uint16_t readAdc(uint8_t pin)
{
  /*
 * Attach pin to ADC (will also clear any other analog mode that could be on)
 * */
//bool adcAttachPin(uint8_t pin);
  if (adcAttachPin(pin))
  {
    Serial.Println (" Attached Pin");
  
  }


/*
 * Start ADC conversion on attached pin's bus
 * */
//bool adcStart(uint8_t pin);
  if (adcStart(pin))
  {
    Serial.Println (" Pin Started");
  
  }

/*
 * Check if conversion on the pin's ADC bus is currently running
 * */
//bool adcBusy(uint8_t pin);
  while (adcBusy(pin))
  {
    Serial.Println (" Pin Busy");
  
  }

/*
 * Get the result of the conversion (will wait if it have not finished)
 * */
//uint16_t adcEnd(uint8_t pin);
  uint16_t val = adcEnd(pin)
  
   Serial.Print (" Pin Done val =");
   Serial.Println ((int)val);
  return val;
  

}
// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
    client.subscribe("mytopic/test", [](const String & payload) {
    Serial.println(payload);
  });

  // Publish a message to "mytopic/test"
  client.publish("mytopic/test", "This is a message"); // You can activate the retain flag by setting the third parameter to true

  // Execute delayed instructions
  client.executeDelayed(5 * 1000, []() {
    client.publish("mytopic/test", "This is a message sent 5 seconds later");
  });
}

int d2val =0 ;
int d2dir = 1;
#define D2RESET 1
int d2cnt = D2RESET;

void loop()
{
  client.loop();
  d2cnt--;
  //if(d2cnt < 0) {
    //Serial.print(" Dac2 output:");
    //Serial.println(d2val);
    dacWrite(DAC2, d2val);
    d2val+=d2dir;
    if(d2val >= 254) d2dir = -1;
    if(d2val <= 1) d2dir = 1;
    d2cnt = D2RESET;
  //}
  
}
