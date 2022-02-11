
#include <Arduino.h>


class Pump {
public:
  Pump(){
    wHeight = 0.0;
    pumpon = 0;
    pumpcur = 0.0;
    raining = 0;
    on_ticks = 0;
    off_ticks = 0;
    ticks = 0;
    pumpOnTime = 5000;
    pumpOffTime = 5000;
    wPRate = 0.003;
    
  }

  ~Pump(){
  }
  
  int showPump(String &sout) {
    String pstate;
    if(pumpon) {
      pstate = " Running";
    } else {
      pstate = "Stopped";
    }
    sout = " Pump :" +String( ticks)+": state "+ pstate
      + " water level :"
      ;
    sout +=  String(wHeight);
    sout += " Current : " + String(pumpcur);
    sout += " on_ticks left  " + String( (on_ticks - ticks));
    sout += " off_ticks left  " + String((off_ticks - ticks));
    sout += "\n"; 
    return 0;
  }
  
  int addWater ( double level) {
    wHeight += level;
  }
  
  int checkWater () {
    if(wHeight> 0.2) return 1 ;
    return 0;
  }
  
  double checkCurrent () {
    return pumpcur ;
  }
  
  void startRain( double rate) {
    raining = 1;
    wRate = rate;
  }
  
  void stopRain() {
    raining = 0;
  }
  
  
  int getPump () {
    return pumpon;
  }
  
  void pumpOn() {
    if(pumpon == 0) {
      on_ticks = ticks + pumpOnTime;
      if(g_debug) {
  //cout<<" Turned on pump at "<<ticks<<" level "<< wHeight<< endl;
      }
      pumpon = 1;
      pumpcur = 2.0;
    }
  }
  
  void pumpOff() {
    if(pumpon == 1) {
      off_ticks = ticks + pumpOffTime;
      if(g_debug) {
  //cout<<" Turned off pump at "<<ticks<<" level "<< wHeight<< endl;
      }
      pumpon = 0;
    }
  }
  
  void runTick(){
    ticks++;
    if(raining) {
      wHeight += wRate;
    }
    if (pumpon) {
      wHeight -= wPRate;
      if(wHeight < 0.2) pumpcur = 0.0;
      // check on_time
      if (ticks > on_ticks) {
  if (wHeight > 0.5 ) {
    pumpOnTime += 1000;
  }
  pumpOff();
      }
      if  (pumpcur < 0.1) {
  pumpOff();
      }
    } else {
      if (ticks > off_ticks) {
  if (wHeight > 0.5 ) {
    pumpOn();
  }
      }
    }
  }
  
  int raining;
  double wRate;
  double wPRate;
  double wHeight;
  int pumpon;
  double pumpcur;
  long int on_ticks;
  long int off_ticks;
  long int ticks;
  long int pumpOnTime;
  long int pumpOffTime;
  
};

