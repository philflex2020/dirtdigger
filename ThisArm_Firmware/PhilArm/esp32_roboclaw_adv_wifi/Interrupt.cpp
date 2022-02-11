#include <Arduino.h>
#include "xtensa/core-macros.h"

//volatile int interruptCounter;
//int totalInterruptCounter;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

uint32_t lastTick = 0;
uint32_t thisTick = 0;
uint32_t maxdiffTick = 0;

const int blueLED = LED_BUILTIN; 
int bluestate = 0;

int toggleLed(uint8_t pin, int &state)
{
   if (state) {
       state = 0;         
       digitalWrite(pin, HIGH);               // GET /H turns the LED on
   } else {
       state = 1;         
       digitalWrite(pin, LOW);               // GET /H turns the LED on
    
   }
}

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer(){
  uint32_t diffTick = 0;
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  thisTick = XTHAL_GET_CCOUNT();
  if (lastTick == 0) {
    lastTick = thisTick;
  } else {
    diffTick = thisTick - lastTick;
    if(diffTick > maxdiffTick) {
      maxdiffTick = diffTick;
    }
  }
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  //xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
  toggleLed(blueLED, bluestate);
}

uint32_t getdiffTick() {
  uint32_t res;
  portENTER_CRITICAL_ISR(&timerMux);
  res = maxdiffTick;
  maxdiffTick = 0;
  portEXIT_CRITICAL_ISR(&timerMux);
  return res;
}
//void setupIRQ(1000000);
void setupIRQ(int itime) {
 
  pinMode(blueLED, OUTPUT); // For LED feedback
  return;
  timer = timerBegin(1, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, itime, true);
  timerAlarmEnable(timer);
 
}
