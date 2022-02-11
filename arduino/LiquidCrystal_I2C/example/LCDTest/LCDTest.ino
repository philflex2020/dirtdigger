#include <Wire.h>


//LCD1602
//You should now see your LCD1602 display the flowing characters "SUNFOUNDER" and "hello, world"
//Email:support@sunfounder.com
//Website:www.sunfounder.com
//2015.5.7
//
//#i//nclude "LiquidCrystal_I2C.h>/include the library code
#include <LiquidCrystal_I2C.h>

/******************************************************#include ****/
char array1[]=" SunFounder               ";  //the string to print on the LCD
char array2[]="hello, world!             ";  //the string to print on the LCD
int tim = 250;  //the value of delay time
// initialize the library with the numbers of the interface pins

LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
/*********************************************************/

void setup()
{
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello, Robot!");
  lcd.setCursor(0,1);
  lcd.print("Powered by Me!");
   //lcd.setCursor(0,2);
  //lcd.print("Arduino LCM IIC 2004");
   //lcd.setCursor(2,3);
  //lcd.print("Power By Ec-yuan!");

  //lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 
}
/*********************************************************/
void loop() 
{
        lcd.scrollDisplayLeft();
delay(1000);

}

//delay(1000);
//    lcd.setCursor(15,0);  // set the cursor to column 15, line 0
//    for ( int positionCounter1 = 0; positionCounter1 < 26; positionCounter1++)
//    {
//      lcd.scrollDisplayLeft();  //Scrolls the contents of the display one space to the left.
//      lcd.print(array1[positionCounter1]);  // Print a message to the LCD.
//      delay(tim);  //wait for 250 microseconds
//    }
//    lcd.clear();  //Clears the LCD screen and positions the cursor in the upper-left corner.
//    lcd.setCursor(15,1);  // set the cursor to column 15, line 1
//    for (int positionCounter2 = 0; positionCounter2 < 26; positionCounter2++)
//    {
//      lcd.scrollDisplayLeft();  //Scrolls the contents of the display one space to the left.
//      lcd.print(array2[positionCounter2]);  // Print a message to the LCD.
//      delay(tim);  //wait for 250 microseconds
//    }
//    lcd.clear();  //Clears the LCD screen and positions the cursor in the upper-left corner.
//}
///************************************************************/
