// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License
//
// This example shows how to generate a JSON document with ArduinoJson.
//
// https://arduinojson.org/v6/example/generator/

//#include <ArduinoJson.h>
#include <Arduino.h>
#include <Servo.h>

int numVals = 23;
int angles[128];
double dist [128];


int g_trigger;
int g_echo;
int g_minRange = -1;
int g_maxRange = -1;
Servo g_serv;

int g_servopin = 9;
int g_servopos = -1;
int servoSetup(int pin)
{
  g_serv.attach(pin);
  g_servopin = pin;
}
// sevo moves at about 90 deg per sec
int servoMove(int pos)
{
   g_serv.write(pos);
   while(g_servopos < pos) {
      delayMicroseconds(10);
      g_servopos += 10;       
   }
   while(g_servopos > pos) {
      delayMicroseconds(10);
      g_servopos -= 10;       
   }
   g_servopos = pos;
}

int sonarSetup(int trigger, int echo)
{
    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
    g_trigger = trigger;
    g_echo = echo;
}

int sonarSetup(int trigger, int echo, int minRange, int maxRange)
{
    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
    g_trigger = trigger;
    g_echo = echo;
    g_minRange = minRange;
    g_maxRange = maxRange;
}

unsigned int echoInMicroseconds()
{
    digitalWrite(g_trigger, LOW);
    delayMicroseconds(5);
    digitalWrite(g_trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(g_trigger, LOW);  
    // Why don't I just use? "return pulseIn(_echo, HIGH);"
  // Arduino Primo doesn't have access to pulseIn.
    while(digitalRead(g_echo) == LOW);

    int pulseStart = micros();

    while(digitalRead(g_echo) == HIGH);

    return micros() - pulseStart;
}

int distanceInMillimeters()
{
    int duration = echoInMicroseconds();
    
    // Given the speed of sound in air is 332m/s = 3320cm/s = 0.0332cm/us).
    int distance = (duration / 2) * 0.332;
  
  if (g_minRange == -1 && g_maxRange == -1)
  {
    return distance;
  }
  
  if (distance > g_minRange && distance < g_maxRange)
  {
    return distance;
  } 
  
  return -1;
}
int getSonar(String &sonar, int vals, int min , int max) {
   
  int i;
  sonar ="{\"Sensor\":\"sonar\",\"Time\":<<TIME>>,\"Values\": [\n";
  for (i = min ; i<max-1;i++) {
    
    // move servo
    String val="["+ String(angles[i])+ "," + String(dist[i]) + "],\n";
    sonar += val;
  }
  String val="["+ String(angles[i])+ "," + String(dist[i]) + "]\n]}\n";
  sonar += val;
}
void setup() {
  // Initialize Serial port
  Serial.begin(115200);
  while (!Serial) continue;

  Serial.println(" jsonsonar version 1.0 - scans sonar \n");
  
  int i;

  // scan servo
  for (int i = 0 ; i<numVals;i++) {
    angles[i] = -40 + (i * 10);
    dist[i] = (45 + (i-2) );
  }
  String sonar;
  // scan servo
  getSonar(sonar, 0 , 0, numVals);
  
  
  Serial.println(sonar);
  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the RAM allocated to this document.
  // Don't forget to change this value to match your requirement.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  //StaticJsonDocument<500> doc;

  // StaticJsonObject allocates memory on the stack, it can be
  // replaced by DynamicJsonDocument which allocates in the heap.
  //
  // DynamicJsonDocument  doc(200);

  // Add values in the document
  //
  //JsonObject root= doc.to<JsonObject>();
  
  //root["sensor"] = "sonar";
  //root["time"] = 1351824120;

  // Add an array.
  //
  //JsonArray pings = root.createNestedArray("pings");
  //JsonArray ping = root.createNestedObject("ping");
  //JsonArray data = doc.createNestedArray("data");
  
  //ping["angle"]= -46.0;
  //ping["dist"] = 144.5;
  //array.add(data);
  //pings.add(ping);
  // Generate the minified JSON and send it to the Serial port.
  //
  //serializeJson(root, Serial);
  // The above line prints:
  // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

  // Start a new line
  Serial.println();

  // Generate the prettified JSON and send it to the Serial port.
  //
  //serializeJsonPretty(doc, Serial);
  // The above line prints:
  // {
  //   "sensor": "gps",
  //   "time": 1351824120,
  //   "data": [
  //     48.756080,
  //     2.302038
  //   ]
  // }
}

void loop() {
  // not used in this example
}

// See also
// --------
//
// https://arduinojson.org/ contains the documentation for all the functions
// used above. It also includes an FAQ that will help you solve any
// serialization problem.
//
// The book "Mastering ArduinoJson" contains a tutorial on serialization.
// It begins with a simple example, like the one above, and then adds more
// features like serializing directly to a file or an HTTP request.
// Learn more at https://arduinojson.org/book/
// Use the coupon code TWENTY for a 20% discount ❤❤❤❤❤
