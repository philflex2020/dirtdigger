/*
  TDS 
  
  change from
  
  Analog Input
  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe
  This example code is in the public domain.
  http://www.arduino.cc/en/Tutorial/AnalogInput
*/

// note i2c uses A4 (SDA) A5(SCL) for I2c comms

#include <Wire.h>
int sensorPin = A3;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue1,sensorValue2,sensorValue3,sensorValue4;  // variable to store the value coming from the sensor

int port1 = 5;      // TDS port1
int port2 = 6;      // TDS port2

uint8_t wdata1[16];
uint8_t wdata2[16];
uint8_t wdata3[16];
uint8_t wdata4[16];
uint8_t wdata5[16];

uint8_t title1[16]="Test 1";
uint8_t title2[16]="Test 2";

size_t wlen = 16;
uint8_t *wdatap;

const int delaytime = 500; //delaytime for discharge

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
//    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  if (x == 0x41) {
      wdatap = title1;
  } else  if (x == 0x42) {
      wdatap = title2;
  }

  else {
        wdatap = wdata3;
  }
  //Serial.println(x);         // print the integer
}

// function that executes whenever data is requested from master
// this function is registered as an event, see setup()
void requestEvent() {
  wdata3[2] = wdata3[2] + 1;
  int x = Wire.write(wdatap, wlen);  // send byte as an integer
 Serial.print(" Sent bytes: ");         // print the integer
 Serial.println(x);         // print the integer

}


void setup() {
  for (unsigned int i = 0; i < wlen; i++ ) {
     wdata1[i] = 0x20;
     wdata2[i] = 0x20;
     wdata3[i] = 0x10 + i;
     wdata4[i] = 0x10 + i;
     wdata5[i] = 0x10 + i;
  }  
  wdata1[0]='T';
  wdata1[1]='e';
  wdata1[2]='s';
  wdata1[3]='t';
  wdata1[4]='1';
  wdata1[15]=0  ;
  wdata2[0]='T';
  wdata2[1]='e';
  wdata2[2]='s';
  wdata2[3]='t';
  wdata2[4]='2';
  wdata2[15]=0;

  
  wdatap = wdata2;
  // setup wire listen on 8
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  // initialize serial:
  Serial.begin(115200);
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  pinMode(port1, OUTPUT);
  pinMode(port2, OUTPUT);
}

void loop() {
  // start
  digitalWrite(ledPin, HIGH);
  
  //port LL
  digitalWrite(port1, LOW);
  digitalWrite(port2, LOW);
  sensorValue1 = analogRead(sensorPin);
  delay(delaytime);
  //port LH
  digitalWrite(port1, LOW);
  digitalWrite(port2, HIGH);
  sensorValue2 = analogRead(sensorPin);
  delay(delaytime);
  
  digitalWrite(ledPin, LOW);
  
  //port HH
  digitalWrite(port1, HIGH);
  digitalWrite(port2, HIGH);
  sensorValue3 = analogRead(sensorPin);
  delay(delaytime);
  //port HL
  digitalWrite(port1, HIGH);
  digitalWrite(port2, LOW);
  sensorValue4 = analogRead(sensorPin);   
  delay(delaytime);
  Serial.print(sensorValue1);
  Serial.print(",");
  Serial.print(sensorValue2);
  Serial.print(",");
  Serial.print(sensorValue3);
  Serial.print(",");
  Serial.print(sensorValue4);
  Serial.println();
}
