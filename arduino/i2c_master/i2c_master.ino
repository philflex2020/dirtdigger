
// I2cMaster

#include <Wire.h>

int Maddr = 9;
void setup()
{
   Wire.begin(); // join i2c bus as master
   Serial.begin(115200);
}

char str[64];
int x = 0;

void loop()
{
   Serial.print(x);
   sprintf(str, "  Message out %7d", x);
   Serial.println(str);
   sprintf(str, "  Message %7d\n", x);
   if (++x > 9999999) x=0;
   Wire.beginTransmission(Maddr); // transmit to device #9
   Wire.write(str);           // sends 16 bytes
   Wire.endTransmission();    // stop transmitting
   Wire.requestFrom(Maddr,8);
   while (Wire.available()) {
     char c = Wire.read();
     Serial.print(c);
   }
   sprintf(str, "Message in %7d\n", x);
   Serial.print(str);
   delay(500);
   
}
