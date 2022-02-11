
#include <Arduino.h>
#include "LiquidCrystal_I2C.h"


int lcdColumns = 20;
int lcdRows = 4;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void show_lcd(int r, int c, String msg)
{
  lcd.setCursor(r, c);
  // print message
  lcd.print(msg);
}

void setup_lcd(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}
