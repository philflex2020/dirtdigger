#include <Arduino.h>

#include "TLM1638_1.h"

const int TLMstrobe = 19;
const int TLMclock = 23;
const int TLMdata = 15;
 
void TLMsendCommand(uint8_t value)
{
  digitalWrite(TLMstrobe, LOW);
  shiftOut(TLMdata, TLMclock, LSBFIRST, value);
  digitalWrite(TLMstrobe, HIGH);
}
 
void TLMreset()
{
  TLMsendCommand(0x40); // set auto increment mode
  digitalWrite(TLMstrobe, LOW);
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOut(TLMdata, TLMclock, LSBFIRST, 0x00);
  }
  digitalWrite(TLMstrobe, HIGH);
}
 
void TLMsetup()
{
  pinMode(TLMstrobe, OUTPUT);
  pinMode(TLMclock, OUTPUT);
  pinMode(TLMdata, OUTPUT);
 
  TLMsendCommand(0x8f);  // activate and set brightness to max
  TLMreset();
}

uint8_t TLMreadButtons(void)
{
  uint8_t buttons = 0;
  digitalWrite(TLMstrobe, LOW);
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0x42);
 
  pinMode(TLMdata, INPUT);
 
  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftIn(TLMdata, TLMclock, LSBFIRST) << i;
    buttons |= v;
  }
 
  pinMode(TLMdata, OUTPUT);
  digitalWrite(TLMstrobe, HIGH);
  return buttons;
}
 
void TLMsetLed(uint8_t value, uint8_t position)
{
  pinMode(TLMdata, OUTPUT);
 
  TLMsendCommand(0x44);
  digitalWrite(TLMstrobe, LOW);
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0xC1 + (position << 1));
  shiftOut(TLMdata, TLMclock, LSBFIRST, value);
  digitalWrite(TLMstrobe, HIGH);
}

void TLMloop()
{
  TLMsendCommand(0x44);  // set single address
 
  digitalWrite(TLMstrobe, LOW);
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0xc0); // 1st digit
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0xff);
  digitalWrite(TLMstrobe, HIGH);
 
  digitalWrite(TLMstrobe, LOW);
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0xc5); // 3rd LED
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0x01);
  digitalWrite(TLMstrobe, HIGH);
 
  digitalWrite(TLMstrobe, LOW);
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0xcb); // 3rd LED
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0x01);
  digitalWrite(TLMstrobe, HIGH);
 
  digitalWrite(TLMstrobe, LOW);
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0xce); // last digit
  shiftOut(TLMdata, TLMclock, LSBFIRST, 0xff);
  digitalWrite(TLMstrobe, HIGH);
}

void TLMloop1()
{
  uint8_t buttons = TLMreadButtons();
 
  for(uint8_t position = 0; position < 8; position++)
  {
    uint8_t mask = 0x1 << position;
 
    TLMsetLed(buttons & mask ? 1 : 0, position);
  }
}
