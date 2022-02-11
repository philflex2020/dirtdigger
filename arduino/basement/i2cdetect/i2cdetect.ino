#include <Wire.h>
#include <i2cdetect.h>

void setup() {
  Wire.begin(0,2);
  Serial.begin(115200);
  Serial.println("i2cdetect example\n");
  Serial.print("Scanning address range 0x03-0x77\n\n");
}

int Scan = 0;
void loop() {
  Serial.print("Scan ..");
  Serial.println(Scan++);
  i2cdetect();  // default range from 0x03 to 0x77
  delay(2000);
}
