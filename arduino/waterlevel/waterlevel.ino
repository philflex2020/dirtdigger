/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED) connected to digital pin 13.
  The amount of time the LED will be on and off depends on the value obtained
  by analogRead().

  The circuit:
  - potentiometer
    center pin of the potentiometer to the analog input 0
    one side pin (either one) to ground
    the other side pin to +5V
  - LED
    anode (long leg) attached to digital output 13
    cathode (short leg) attached to ground

  - Note: because most Arduinos have a built-in LED attached to pin 13 on the
    board, the LED is optional.

  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInput
*/

int sensorPin = A7;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
int lastValue = 0;  // variable to store the value coming from the sensor
int Valdiff = 0;
int Delay;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Delay =  1000;
   // initialize serial:
  Serial.begin(115200);
}

void loop() {
  Delay = 1000;
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  // turn the ledPin on
  if(sensorValue > 200) {
       Delay = 1000;
       digitalWrite(ledPin, HIGH);
  }
  
  // stop the program for <sensorValue> milliseconds:
  if(sensorValue < 110) {
       //delay(3000);
       Delay = 2000;
    // turn the ledPin off:
    digitalWrite(ledPin, LOW);
  }
  Valdiff = 0;
  if (lastValue > 0 ) {
    Valdiff = lastValue - sensorValue;
    //lastValue = sensorValue;
  }
  if (Valdiff < -200) {
       Delay = 2000;
    // turn the ledPin off:
    digitalWrite(ledPin, LOW);
  }
  
  lastValue = sensorValue;
  //look for dropping alue
  // stop the program for <sensorValue> milliseconds:
  if(sensorValue-Valdiff < 130) {
       //delay(3000);
       Delay = 2000;
    // turn the ledPin off:
    digitalWrite(ledPin, LOW);
  }
  if(Valdiff > 130) {
       //delay(3000);
       Delay = 2000;
    // turn the ledPin off:
    digitalWrite(ledPin, LOW);
  }

  // stop the program for for <sensorValue> milliseconds:
  delay(Delay);
  Serial.print("Value :");
  Serial.print(sensorValue);
  Serial.print(" Diff :");
  Serial.print(Valdiff);
  Serial.println();
}
