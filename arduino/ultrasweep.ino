/This demo is used for testing the ultrasonic module.
//Connect the module to Arduino UNO trig -> A2  echo ->A3
//When the code start, it will print the value of the ultrasonic
//to the serial montor

#include <Servo.h>

#define SWEEP_PIN 10
#define POINT_PIN 9

#define TRIG_PIN A2
#define ECHO_PIN A3

Servo Ssweep;
Servo Spoint;
int SweepInc = 5;
int SweepMax = 120;
int SweepMin = 50;
int SweepPos = 50;
int SweepIdx = 0;
int stemp[128];
int stempMin = 2048;
int stempMax = 0;
int Savg=0;
void setup()
{
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT); //Set the connection pin output mode Echo pin
  pinMode(TRIG_PIN, OUTPUT);//Set the connection pin output mode trog pin
  Ssweep.attach(SWEEP_PIN);
  Spoint.attach(POINT_PIN);
  Ssweep.write(SweepPos);
  for (int i = 0; i < 128; i++)
   stemp[i]=2048;
   
}

void loop(){
  
  int temp = readPing();
  temp += readPing();
  temp += readPing();
  temp += readPing();
  temp += readPing();
  temp /= 5;
  stemp[SweepIdx] = temp;
  if(temp > 100) temp = 200;
  if (SweepIdx < stempMin) stempMin = SweepIdx;
  if (SweepIdx > stempMax) stempMax = SweepIdx;
  Serial.print(SweepIdx *5,DEC);
  Serial.print(",");
  Serial.print(Savg,DEC);
  Serial.print(",");
  Serial.println(temp,DEC);
  SweepPos += SweepInc;
  if(SweepInc > 0) SweepIdx += 1;
  if(SweepInc < 0) SweepIdx -= 1;
  if(SweepPos >= SweepMax) SweepInc = - SweepInc;
  if(SweepPos <= SweepMin) SweepInc = - SweepInc;
  Ssweep.write(SweepPos);
  runScan();
  //Spoint.write(SweepPos);
  delay(50);
  
  
  }

int readPing()
{
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, cm;
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, LOW);

  pinMode(ECHO_PIN, INPUT);
  duration = pulseIn(ECHO_PIN, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  return cm ;
}


int runScan (void)
{
  int iavg = 0;
  int ipos;
  if(SweepPos >= SweepMax) {
    for (int i = stempMin; i < stempMax; i++) {
      iavg += stemp[i];
    } 
    iavg = iavg / (stempMax-stempMin);
    Savg = iavg;
  
    for (int i = stempMin; i < stempMax; i++) {
      if(stemp[i] > iavg) {
  //      Savg = stemp[i];
        ipos = i * 5;
        Spoint.write(ipos);        
      }
    } 

   }
}
long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
