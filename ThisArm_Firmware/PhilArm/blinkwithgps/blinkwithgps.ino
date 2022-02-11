/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
#include<TinyGPS++.h>

// Connect the GPS Power pin to 3.3V
// Connect the GPS Ground pin to ground
// Connect the GPS VBAT pin to 3.3V if no battery is used
// Connect the GPS TX (transmit) pin to Digital 2
// Connect the GPS RX (receive) pin to Digital 3
// For 3.3V only modules such as the UP501, connect a 10K
// resistor between digital 3 and GPS RX and a 10K resistor 
// from GPS RX to ground.

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

// to generate your own sentences, check out the MTK command datasheet and use a checksum calculator
// such as the awesome http://www.hhhh.org/wiml/proj/nmeaxor.html

#define LED_ESP32 2

#define RXD2 16 
#define TXD2 17

static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);        // Start the Serial communication to send messages to the computer
  Serial2.begin(GPSBaud, SERIAL_8N1, RXD2, TXD2);

  delay(10);
  Serial.println("file --- blinkwithGps");
  Serial.println("vesion 1.01");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_ESP32, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("ON");
  //Serial2.println("2..on");
  digitalWrite(LED_ESP32, HIGH);   // turn the LED on (HIGH is the voltage level)
  while(Serial2.available())
    Serial.print(char(Serial2.read()));

  delay(1000);                       // wait for a second
  Serial.println("OFF");
  digitalWrite(LED_ESP32, LOW);    // turn the LED off by making the voltage LOW
  //Serial2.println("2..on");
  while(Serial2.available())
      Serial.print(char(Serial2.read()));

  delay(1000);                       // wait for a second
  
}
