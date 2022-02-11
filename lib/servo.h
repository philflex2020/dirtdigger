//// servo class driver

#ifndef __SERVO_H
#define __SERVO_H

#include "sock.h"
#include "array.h"
#include "tests.h"
#include "vars.h"
#include "thread.h"


using namespace std;
struct Servo;
struct Thread;

int setupServoCmd(void *tdest, Servo *s, Array *cmd);
int sendServoCmd(void *tdest, Servo *s, Array *cmd);
int runSingleServo(Servo *s);

struct Move {
  Move(int xpin, int xdest, int xtime) {
    pin = xpin;
    dest =  xdest;
    time = xtime;
  };
  ~Move(){};
  int pin;
  int dest;
  int time;
};
 
struct Servo : public Var {
  Servo(int pin);
  Servo(char *sname, char *spin, char *spos);
  ~Servo();
  int init(int pin);
  
  void setVal(int val, int time);
  void setVal(char *spos, char *stime);
  void setMove(char *spos, char *stime);
  Thread *setUpRun(void *tdest, int run);

  int addMove(Move *mv) {
    if(movArray == NULL) {
      movArray = new Array(16,16, "Moves");
    }
    movArray->pushItem(mv);
    return 0;
  };

  int nextMove(void) {
    Move *mv = (Move *)movArray->popItem();
    if (mv) {
      setVal(mv->dest, mv->time);
      delete mv;
    }
    return 0;
  }

  int sendCmd(void *tdest, Array *cmd) {  
    return sendServoCmd(tdest, this, cmd);
  }
  string name;
  int pin;

  int pos;
  int base;
  int rpos;
  int time;
  int incr;
  int pc;


  int on;
  int off;
  int count;
  Array *movArray;
  int id;
  
};






#endif
