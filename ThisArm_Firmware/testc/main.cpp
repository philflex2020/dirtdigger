#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string.h>
typedef uint8_t byte;

using namespace std;


//encoder definition
#define Encoder1_A 12
#define Encoder1_B 13
#define Encoder1_Click 11

#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define A4 4
#define INPUT_PULLUP 0x80

int pinMode(int i, int j)
{
  return 0;
}

char *F(string g)
{
  return (char *)g.c_str();
}
byte Drag_Flag=0;//flag indicate whether a Drag action occured
long ReleaseTurnCount =0;
long PressTurnCount =0;
long ClickCount=0;
long DubleClickCount=0;
byte prevClick=0;
byte currClick=0;
unsigned long PrevClick_milliSec=0;
unsigned long ThisClick_milliSec=0;
unsigned int DoubleClick_Threshold_millisec=300; // detect as "Double Click " if two click within 300 ms

//Encoder Encoder1 (Encoder1_A,Encoder1_B);

//Control Loop set-up
#define MainLoopPeriod_MicroSecond 2500 //2.5 milli second
#define Loop200Hz_overflowVal 5000/MainLoopPeriod_MicroSecond
#define Loop100Hz_overflowVal 10000/MainLoopPeriod_MicroSecond
#define Loop25Hz_overflowVal 40000/MainLoopPeriod_MicroSecond

unsigned int counter_25Hz=0;
unsigned int counter_100Hz=0;
unsigned int counter_200Hz=0;
unsigned long timer=0;   //general purpuse timer
unsigned long timer_old;
unsigned long executionTime = 8;
unsigned long executionTimeStart = 0;
unsigned long executionTimeMirf = 0;
unsigned long G_Dt=2500;    // Integration time (DCM algorithm)

//ControlCounter
unsigned int servoSelected=0;
unsigned int analogJointSelected=0;

//JointControl, angle in Degree
double BaseAngle =90;
double Shoulder=90;
double Elbow=90;
double Claw=45;

//Invert Kinetic control, in polar coordinate R, H and A
double R;
double H;
double A;

//Cartesian control, in XYZ coordinate
double X=00;
double Y=80;
double Z=0;

typedef  class {
public:
    void begin(...) {}
    void print(...) {}
    void write(...) {}
    void println(...) {}
    void attach(...) {}
} Servo;

#ifndef ENABLE_PRINT
// disable Serial output
#define Serial SomeOtherwiseUnusedName
static class {
public:
    void begin(...) {}
    void print(...) {}
    char read(...) {}
    int available(...) {}
    void println(...) {}
} Serial;
#endif

int delay(int t)
{
  return 0;
}
int millis(void)
{
  return 0;
}

int digitalRead(int val)
{
  return 0;
}
//#include "Encoder.h"
#include "Transformation.h"
#include "AngularServo.h"
#include "AnalogControl.h"
#include "ControlFunctions.h"
#include "DisplayFunctions.h"
#include "GCode.h"
#include "InitFunctions.h"
#include "StageContainer.h"
#include "CoordinateControl.h"

#include "ControlFunctions.ino"
#include "DisplayFunctions.ino"
#include "InitFunctions.ino"
#include "GCode.ino"

//#include "InitFunctions.h"
//#include "ControlFunctions.h"
//#include "DisplayFunctions.h"
//#include "CoordinateControl.h"

//#include "AnalogControl.h"
//#include "GCode.ino"
int main (int argc, char * argv[])
{
  Serial.println("foo 21 is me ");
  
  cout<< " hello 21 from main"<<endl;
  return 0;
}
