
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Servo.h>

MPU6050 mpu6050(Wire);
Servo angX;
Servo angY;
#define ANGX_PIN 6
#define ANGY_PIN 7

int angx = 90;
int angy = 10;

long timer = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  angX.attach(ANGX_PIN);
  angY.attach(ANGY_PIN);
  angX.write(angx);
  angY.write(angy);
  
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}
int timew =250;
void loop() {
  mpu6050.update();

  if(millis() - timer > timew){
    
    Serial.println("=======================================================");
    Serial.print("temp : ");Serial.println(mpu6050.getTemp());
    Serial.print("accX : ");Serial.print(mpu6050.getAccX());
    Serial.print("\taccY : ");Serial.print(mpu6050.getAccY());
    Serial.print("\taccZ : ");Serial.println(mpu6050.getAccZ());
  
    Serial.print("gyroX : ");Serial.print(mpu6050.getGyroX());
    Serial.print("\tgyroY : ");Serial.print(mpu6050.getGyroY());
    Serial.print("\tgyroZ : ");Serial.println(mpu6050.getGyroZ());
  
    Serial.print("accAngleX : ");Serial.print(mpu6050.getAccAngleX());
    Serial.print("\taccAngleY : ");Serial.println(mpu6050.getAccAngleY());
  
    Serial.print("gyroAngleX : ");Serial.print(mpu6050.getGyroAngleX());
    Serial.print("\tgyroAngleY : ");Serial.print(mpu6050.getGyroAngleY());
    Serial.print("\tgyroAngleZ : ");Serial.println(mpu6050.getGyroAngleZ());
    
    Serial.print("angleX : ");Serial.print(mpu6050.getAngleX());
    Serial.print("\tangleY : ");Serial.print(mpu6050.getAngleY());
    Serial.print("\tangleZ : ");Serial.println(mpu6050.getAngleZ());
    Serial.println("=======================================================\n");
    angx = mpu6050.getAngleX()+90;
    angy = mpu6050.getAngleY()+90;
    if(angx > 180) {
      angx = 170;
    }
    if(angy > 180) {
      angy = 170;
    }
    if(angx < 10) {
      angx = 10;
    }
    if(angy < 10) {
      angy = 10;
    }
    
    angX.write(angx);
    angY.write(angy);

    timer = millis();
    
  }

}
