#pragma once
#include <Arduino.h>
#include <driver/gpio.h>
#include "driver/pcnt.h"
#define MAX_ESP32_ENCODERS PCNT_UNIT_MAX
class ESP32Encoder {
private:
  void attach(int aPintNumber, int bPinNumber, boolean fullQuad);
  boolean attached=false;


  static  pcnt_isr_handle_t user_isr_handle; //user's ISR service handle
    bool direction;
    bool working;
public:
  ESP32Encoder();
  ~ESP32Encoder();
  void attachHalfQuad(int aPintNumber, int bPinNumber);
  void attachSingleEdge(int aPintNumber, int bPinNumber);
  //void attachHalfQuad(int aPintNumber, int bPinNumber);
  int32_t getCount();
  int32_t getCountRaw();

  boolean isAttached(){return attached;}
  void setCount(int32_t value);
  static ESP32Encoder *encoders[MAX_ESP32_ENCODERS];
  static bool attachedInterrupt;
  gpio_num_t aPinNumber;
  gpio_num_t bPinNumber;
  pcnt_unit_t unit;
  bool fullQuad=false;
  int countsMode = 2;
  volatile int32_t count=0;
  pcnt_config_t r_enc_config;
};

#if 0
// on philwil github

// example
#include <ESP32Encoder.h>

ESP32Encoder encoder;
ESP32Encoder encoder2;

// timer and flag for example, not needed for encoders
unsigned long encoder2lastToggled;
bool encoder2Paused = false;

void setup(){
  
  Serial.begin(115200);
  // Enable the weak pull down resistors
  ESP32Encoder::useInternalWeakPullResistors=true;
  
  // set starting count value
  encoder.setCount(37);

  // clear the encoder's raw count and set the tracked count to zero
  encoder2.clearCount();

  Serial.println("Encoder Start = "+String((int32_t)encoder.getCount()));

  // Attache pins for use as encoder pins
  encoder.attachHalfQuad(36, 39);
  // Attache pins for use as encoder pins
  encoder2.attachHalfQuad(34, 35);

  // set the lastToggle
  encoder2lastToggled = millis();
}

void loop(){
  // Loop and read the count
  Serial.println("Encoder count = "+String((int32_t)encoder.getCount())+" "+String((int32_t)encoder2.getCount()));
  delay(100);

  // every 5 seconds toggle encoder 2
  if (millis() - encoder2lastToggled >= 5000) {
    if(encoder2Paused) {
      Serial.println("Resuming Encoder 2");
      encoder2.resumeCount();
    } else {
      Serial.println("Paused Encoder 2");
      encoder2.pauseCount();
    }

    encoder2Paused = !encoder2Paused;
    encoder2lastToggled = millis();
  }
}
#endif
